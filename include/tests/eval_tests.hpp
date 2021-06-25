#ifndef EVAL_TESTS_HPP
#define EVAL_TESTS_HPP

#include "../lexer/lexer_builder.hpp"
#include "../parser/parser.hpp"
#include "../expressions/eval.hpp"

extern std::ostream& default_stdout;

namespace eval_detail {

void run_test(std::string_view program, std::string_view expected_output)
{
    static int test_counter = 0;

    std::cout << "Run test " << test_counter++ << " => ";

    Lexer lexer = LexerBuilder{}
        .keywords(test_keywords)
        .operators(test_operators)
        .input(std::istringstream{program.data()})
        .build();

    Parser parser(lexer.tokenize());

    auto parsed_program = parser.parse();

    SemanticAnalyzer semantic_analyzer(parsed_program);
    semantic_analyzer.analyze();

    Evaluator evaluator(parsed_program);

    evaluator.eval();

    try
    {
        auto& string_stream = dynamic_cast<std::ostringstream&>(default_stdout);

        if (string_stream.str() != expected_output)
        {
            std::cerr << "EVAL ERROR: for " << program << "\n\tgot [" << string_stream.str() << "], expected [" << expected_output << "]\n";
            exit(-1);
        }

        string_stream.str("");

    } catch (std::bad_cast&) {

    }

    default_stdout.clear();

    std::cout << "OK\n";
}

void expect_error(std::string_view program, std::string_view expected_output)
{
    try
    {
        Lexer lexer = LexerBuilder{}
            .keywords(test_keywords)
            .operators(test_operators)
            .input(std::istringstream{program.data()})
            .build();

        Parser parser(lexer.tokenize());

        auto parsed_program = parser.parse();

        SemanticAnalyzer semantic_analyzer(parsed_program);
        semantic_analyzer.analyze();

        Evaluator evaluator(parsed_program);

        evaluator.eval();

    } catch (LexicalError& lexical_error) {

        std::cout << "While analyzing:\n\t" << program << "\nLexical error processed:\n\t" << lexical_error.what() << "\n\n";
        return;

    } catch (ParseError& parse_error) {

        std::cout << "While analyzing:\n\t" << program << "\nParse error processed:\n\t" << parse_error.what() << "\n\n";
        return;

    } catch (SemanticError& semantic_error) {

        std::cout << "While analyzing:\n\t" << program << "\nSemantic error processed\n\t" << semantic_error.what() << "\n\n";
        return;

    } catch (EvalError& eval_error) {

        std::cout << "While analyzing:\n\t" << program << "\nEval error processed\n\t" << eval_error.what() << "\n\n";
        return;
    }
}

} // namespace eval_detail

void run_eval_tests()
{
    eval_detail::run_test("fun main() { print(1); }",
        "1");
    eval_detail::run_test("fun main() { print(1 + 1); }",
        "2");
    eval_detail::run_test("fun simple() { var = 2; var; } fun main() { print(simple()); }",
        "2");
    eval_detail::expect_error("fun simple() { { var = 2; } var; } fun main() { simple(); }",
        "");
    eval_detail::run_test("fun return_string() { \"String\"; } fun main() { print(return_string()); }",
        "String");
    eval_detail::run_test("fun simple() { var = \"Text\"; var; } fun main() { var = simple(); print(var); }",
        "Text");
    eval_detail::run_test("fun ret() { 1 + 1 + 1 + 1; } fun test() { var = 2; var + ret(); } fun main() { print(test()); } ",
        "6");
    eval_detail::run_test("fun ret(var) { var; } fun main() { print(ret(123));}",
        "123");
    eval_detail::run_test("fun create_int() { 1; } fun sum(lhs, rhs) { lhs + rhs; } fun main() { print(sum(1 + 1, create_int())); }",
        "3");
    eval_detail::run_test("fun main() { var = 1; var = var + 1; print(var); }",
        "2");
    eval_detail::run_test("fun main() { var = 0; while (var != 10) { var = var + 1; print(var); if (var < 10) { print(\" \"); } } }",
        "1 2 3 4 5 6 7 8 9 10");
    eval_detail::run_test("fun main() { var = 0; if (var == 0) { println(\"Equal\"); } else { println(\"Different\"); } }",
        "Equal\n");
    eval_detail::run_test("fun main() { var = 0; if (var != 0) { println(\"Equal\"); } else { println(\"Different\"); } }",
        "Different\n");
}

#endif // EVAL_TESTS_HPP
