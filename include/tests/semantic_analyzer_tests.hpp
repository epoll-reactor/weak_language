#ifndef SEMANTIC_ANALYZER_TESTS_HPP
#define SEMANTIC_ANALYZER_TESTS_HPP

#include "../semantic/semantic_analyzer.hpp"
#include "../tests/parser_tests.hpp"

namespace semantic_detail {

void expect_error(std::string_view data)
{
    try
    {
        const std::shared_ptr<ast::RootObject> parsed_trees = parser_detail::create_parse_tree_impl(data);

        SemanticAnalyzer analyzer(parsed_trees);

        analyzer.analyze();

    } catch (LexicalError& lex_error) {

        std::cout << "While analyzing:\n\t" << data << "\nLexical error processed:\n\t" << lex_error.what() << "\n\n";
        return;

    } catch (ParseError& parse_error) {

        std::cout << "While analyzing:\n\t" << data << "\nParse error processed:\n\t" << parse_error.what() << "\n\n";
        return;

    } catch (SemanticError& semantic_error) {

        std::cout << "While analyzing:\n\t" << data << "\nSemantic error processed:\n\t" << semantic_error.what() << "\n\n";
        return;

    } catch (...) {

        std::cout << "While analyzing:\n\t" << data << "\nUnknown error processed:\n\t\n\n";
        return;
    }

    const bool error_expected = false;

    assert(error_expected);
}

void assert_correct(std::string_view data)
{
    const std::shared_ptr<ast::RootObject> parsed_trees = parser_detail::create_parse_tree_impl(data);

    SemanticAnalyzer analyzer(parsed_trees);
    analyzer.analyze();
}

void analyze_variables(std::string_view data, std::size_t scope_depth, std::string_view variable_name, std::string_view expected_value)
{
    const std::shared_ptr<ast::RootObject> parsed_trees = parser_detail::create_parse_tree_impl(data);

    SemanticAnalyzer analyzer(parsed_trees);
    analyzer.analyze();

//    SymbolTable::SymbolInfo info = analyzer.find_variable(variable_name);

//    assert(info.name == expected_value);

//    std::cout << "Found: " << info.name << ", depth = " << info.depth << '\n';
}

} // namespace semantic_detail

void semantic_analyzer_test_syntax()
{
    semantic_detail::expect_error("if (1) {} else { if (2) {} else { if (\"Non-bool\") {} else {} } }");
    semantic_detail::expect_error("1 ++ 2;");
    semantic_detail::expect_error("if (while (1) {}) {}");
    semantic_detail::expect_error("{ { { if (while (1) {}) {} } } }");

    semantic_detail::assert_correct("while (1) { if (1) {} else { while (1 == 1) {} } }");

    semantic_detail::expect_error("1 = Symbol;");
    semantic_detail::expect_error("1 = while (1) {};");
    semantic_detail::assert_correct("Symbol = 1;");
    semantic_detail::assert_correct("Symbol = \"\";");
    semantic_detail::assert_correct("Symbol = 1 + 1;");

    semantic_detail::expect_error("while (1 = 1) {}");
    semantic_detail::assert_correct("if (1 + 2 + 3) {}");
    semantic_detail::expect_error("if (1 + 2 = 3) {}");

    semantic_detail::expect_error("1 + 2 = 3;");

    semantic_detail::assert_correct("Symbol /= 2;");
    semantic_detail::assert_correct("Symbol /= 1 * 2 / 3 * 4 / 5 * 6 / 7 * 8 / 9;");
    semantic_detail::expect_error("Symbol = 1 * 2 *= 3;");

    semantic_detail::assert_correct("fun compound(a, b, c) { while (1) { if (1) {} else { while (1 == 1) {} } } }");
    semantic_detail::expect_error("fun compound(a, b, c) { if (1) {} else { if (2) {} else { if (\"Non-bool\") {} else {} } } }");

    semantic_detail::assert_correct("fun simple(a, b, c) {}");
    semantic_detail::expect_error("fun simple(1, 2, 3) {}");
    semantic_detail::expect_error("simple(fun inner() {});");
    semantic_detail::expect_error("simple(if (1) {} else {});");
}

void semantic_analyzer_test_variables()
{
    semantic_detail::analyze_variables(
        /* Expression     */ "Symbol = 222;",
        /* Depth          */ 1,
        /* Variable name  */ "Symbol",
        /* Expected value */ "222");

    semantic_detail::analyze_variables(
        "{ Symbol2 = 222; }",
        2,
        "Symbol2",
        "222");
}

void run_semantic_analyzer_tests()
{
    std::cout << "Running semantic analyzer tests...\n====\n";

    semantic_analyzer_test_syntax();
    semantic_analyzer_test_variables();

    std::cout << "Semantic analyzer tests passed successfully\n";
}

#endif // SEMANTIC_ANALYZER_TESTS_HPP