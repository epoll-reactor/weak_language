#ifndef EVAL_TESTS_HPP
#define EVAL_TESTS_HPP

#include "../lexer/lexer_builder.hpp"
#include "../parser/parser.hpp"
#include "../eval/eval.hpp"
#include "../eval/eval_error.hpp"

extern std::ostream& default_stdout;

namespace eval_detail {

static int test_counter = 0;

void run_test(std::string_view program, std::string_view expected_output)
{
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

void expect_error(std::string_view program)
{
    std::cout << "Run fuzz test " << test_counter++ << " => ";

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
        goto clear_stdout;

    } catch (ParseError& parse_error) {

        std::cout << "While analyzing:\n\t" << program << "\nParse error processed:\n\t" << parse_error.what() << "\n\n";
        goto clear_stdout;

    } catch (SemanticError& semantic_error) {

        std::cout << "While analyzing:\n\t" << program << "\nSemantic error processed\n\t" << semantic_error.what() << "\n\n";
        goto clear_stdout;

    } catch (EvalError& eval_error) {

        std::cout << "While analyzing:\n\t" << program << "\nEval error processed\n\t" << eval_error.what() << "\n\n";
        goto clear_stdout;
    }

clear_stdout:
    try
    {
        auto& string_stream = dynamic_cast<std::ostringstream&>(default_stdout);

        string_stream.str("");

    } catch (std::bad_cast&) { }

    default_stdout.clear();
}

} // namespace eval_detail

void run_eval_tests()
{
    std::cout << "Running eval tests...\n====\n";

    eval_detail::run_test("fun main() { print(1); }",
        "1");
    eval_detail::run_test("fun main() { print(1 + 1); }",
        "2");
    eval_detail::run_test("fun simple() { var = 2; var; } fun main() { print(simple()); }",
        "2");
    eval_detail::expect_error("fun simple() { { var = 2; } var; } fun main() { simple(); }");

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
    eval_detail::run_test("fun main() { for (i = 0; i < 10; i = i + 1) { print(i); } }",
        "0123456789");
    eval_detail::run_test("fun main() { for (i = 0; i < 100000; i = i + 1) { } }",
        "");
    eval_detail::run_test("fun copy(arg) { arg; } fun main() { for (i = 0; i < 10; i = i + 1) { print(copy(i)); } }",
        "0123456789");
    eval_detail::expect_error("fun main() { for (var = 0; var != 10; var = var + 1) { } print(var); }");
    eval_detail::expect_error("fun main() { for (var = 0; var != 10; var = var + 1) { for (var_2 = 0; var_2 != 10; var_2 = var_2 + 1) { print(var); } print(var_2); } }");

    eval_detail::run_test("fun main() { var = 0; print(number?(var)); print(string?(var)); }",
        "10");
    eval_detail::run_test("fun main() { var = \"0\"; print(number?(var)); print(string?(var)); }",
        "01");
    eval_detail::run_test("load \"examples/nested.wl\"; fun main() { print(test(), test2()); }",
        "1 2");

    eval_detail::run_test(R"__(
        fun sqrt(x) {
            temp = 0;
            number = x;
            root = number / 2;

            while (root != temp) {
                temp = root;

                tmp_value = number / temp;
                tmp_value = tmp_value + temp;
                tmp_value = tmp_value / 2;
                root = tmp_value;
            }

            root;
        }

        fun main() {
            print(sqrt(9), sqrt(16), sqrt(25), sqrt(36), sqrt(49), sqrt(64), sqrt(81));
        }
    )__", "3 4 5 6 7 8 9");
    eval_detail::run_test(R"__(
        fun power(num, stage) {
            result = 1;

            while (stage > 0) {
                result = result * num;

                stage = stage - 1;
            }

            result;
        }

        fun main() {
            print(power(2, 1), power(2, 2), power(2, 3), power(2, 4), power(2, 5), power(2, 6), power(2, 7), power(2, power(2, 3)));
        }
    )__", "2 4 8 16 32 64 128 256");

    std::cout << "Eval tests passed successfully\n";
}

#endif // EVAL_TESTS_HPP
