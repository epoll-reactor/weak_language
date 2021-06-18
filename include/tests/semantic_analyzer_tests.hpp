#ifndef SEMANTIC_ANALYZER_TESTS_HPP
#define SEMANTIC_ANALYZER_TESTS_HPP

#include "../semantic/semantic_analyzer.hpp"
#include "../tests/parser_tests.hpp"

namespace semantic_detail {

void expect_error(std::string_view data)
{
    const std::shared_ptr<ast::RootObject> parsed_trees = parser_detail::create_parse_tree_impl(data);

    SemanticAnalyzer analyzer(parsed_trees);

    try
    {
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
} // namespace semantic_detail

void run_semantic_analyzer_tests()
{
    semantic_detail::expect_error("if (1) {} else { if (2) {} else { if (\"Non-bool\") {} else {} } }");
    semantic_detail::expect_error("1 ++ 2;");
    semantic_detail::expect_error("if (while (1) {}) {}");

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
}

#endif // SEMANTIC_ANALYZER_TESTS_HPP