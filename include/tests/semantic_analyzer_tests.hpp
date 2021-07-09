#ifndef SEMANTIC_ANALYZER_TESTS_HPP
#define SEMANTIC_ANALYZER_TESTS_HPP

#include "../lexer/lexer.hpp"
#include "../semantic/semantic_analyzer.hpp"
#include "../parser/parser.hpp"
#include "../tests/test_utility.hpp"

namespace semantic_detail {

std::shared_ptr<ast::RootObject> create_parse_tree(std::string_view data)
{
    Lexer lexer(std::istringstream{data.data()});

    Parser parser(lexer.tokenize());

    return parser.parse();
}

void expect_error(std::string_view data)
{
    trace_error(data, [&data]{
        const std::shared_ptr<ast::RootObject> parsed_trees = create_parse_tree(data);

        SemanticAnalyzer analyzer(parsed_trees);

        analyzer.analyze();

        /// Will be skipped if exception thrown from analyzer function
        assert(false && "Error expected");
    });
}

void assert_correct(std::string_view data)
{
    const std::shared_ptr<ast::RootObject> parsed_trees = create_parse_tree(data);

    SemanticAnalyzer analyzer(parsed_trees);
    analyzer.analyze();
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
    semantic_detail::assert_correct("while (12.34) {}");
    semantic_detail::assert_correct("if (1 + 2 + 3) {}");
    semantic_detail::expect_error("if (1 + 2 = 3) {}");

    semantic_detail::assert_correct("for (;;) {}");
    semantic_detail::assert_correct("for (i = 0; i < 10; i = i + 1) {}");
    semantic_detail::expect_error("for (i == 0; i < 10; i = i + 1) {}");
    semantic_detail::expect_error("for (i = 0; while (1) {}; i = i + 1) {}");
    semantic_detail::expect_error("for (i = 0; i < 10; \"What\") {}");

    semantic_detail::expect_error("1 + 2 = 3;");

    semantic_detail::assert_correct("Symbol /= 2;");
    semantic_detail::assert_correct("Symbol /= 1 * 2 / 3 * 4 / 5 * 6 / 7 * 8 / 9;");
    semantic_detail::expect_error("Symbol = 1 * 2 *= 3;");

    semantic_detail::assert_correct("fun compound(a, b, c) { while (1) { if (1) {} else { while (1 == 1) {} } } }");
    semantic_detail::expect_error("fun compound(a, b, c) { if (1) {} else { if (2) {} else { if (\"Non-bool\") {} else {} } } }");

    semantic_detail::assert_correct("fun simple(a, b, c) {}");
    semantic_detail::expect_error("fun simple(1, 2, 3) {}");
    semantic_detail::expect_error("fun simple(fun simple() {}) {}");
    semantic_detail::expect_error("simple(fun inner() {});");
    semantic_detail::expect_error("simple(if (1) {} else {});");
    semantic_detail::assert_correct("simple(1);");
    semantic_detail::assert_correct("simple(\"text\");");
    semantic_detail::assert_correct("simple_1(simple_2());");

    semantic_detail::assert_correct("array[1];");
    semantic_detail::assert_correct("array[1 + 1];");
    semantic_detail::assert_correct("array[name];");
    semantic_detail::assert_correct("array[function_call()];");
    semantic_detail::expect_error("array[\"String\"];");
    semantic_detail::expect_error("array[fun main() {}];");
    semantic_detail::expect_error("array[1.42];");
}

void run_semantic_analyzer_tests()
{
    std::cout << "Running semantic analyzer tests...\n====\n";

    semantic_analyzer_test_syntax();

    std::cout << "Semantic analyzer tests passed successfully\n";
}

#endif // SEMANTIC_ANALYZER_TESTS_HPP
