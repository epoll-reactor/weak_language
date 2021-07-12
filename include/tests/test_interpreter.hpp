#ifndef WEAK_TESTS_INTERPRETER_HPP
#define WEAK_TESTS_INTERPRETER_HPP

#include "../error/eval_error.hpp"
#include "../eval/eval.hpp"
#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"
#include "../semantic/semantic_analyzer.hpp"

#include "../tests/test_utility.hpp"

#include <sstream>
#include <iostream>

extern std::ostream& default_stdout;

namespace eval_detail {

static int test_counter = 0;

void run_test(std::string_view program, std::string_view expected_output)
{
    std::cout << "Run test " << test_counter++ << " => ";

    Lexer lexer(std::istringstream{program.data()});

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

    bool error = true;

    trace_error(program, [&error, &program]{
        Lexer lexer(std::istringstream{program.data()});

        Parser parser(lexer.tokenize());

        auto parsed_program = parser.parse();

        SemanticAnalyzer semantic_analyzer(parsed_program);
        semantic_analyzer.analyze();

        Evaluator evaluator(parsed_program);

        evaluator.eval();

        /// Will false if exception thrown
        error = false;
    });

    assert(error);
}

} // namespace eval_detail

void run_eval_tests()
{
    std::cout << "Running eval tests...\n====\n";

    eval_detail::run_test("fun main() {}", "");
    eval_detail::run_test("fun main() { print(1); }", "1");
//    eval_detail::run_test("fun main() { print(2 + 2 * 2 * 2); }", "10");
//    eval_detail::run_test("fun main() { print(2 * 2 * 2 + 2); }", "10");
    eval_detail::run_test("fun main() { print(1 + 1.5); }", "2.5");
    eval_detail::run_test("fun main() { print(1.5 + 1); }", "2.5");
    eval_detail::run_test("fun main() { print(1.5 + 1.5); }", "3");
    eval_detail::run_test("fun main() { print(123 % 7); }", "4");
    eval_detail::run_test("fun main() { print(2 << 2, 2 << 9, 2 << 10); }", "8 1024 2048");
    eval_detail::run_test("fun main() { print(1 * 2 * 3 * 4 * 5); }", "120");
    eval_detail::run_test("fun main() { print(++1); }", "2");
    eval_detail::run_test("fun main() { print(--1); }", "0");
    eval_detail::run_test("fun main() { var = 10; print(--var); }", "9");
    eval_detail::run_test("fun simple() { var = 2; var; } fun main() { print(simple()); }", "2");
    eval_detail::run_test("fun return_string() { \"String\"; } fun main() { print(return_string()); }", "String");
    eval_detail::run_test("fun simple() { var = \"Text\"; var; } fun main() { var = simple(); print(var); }", "Text");
    eval_detail::run_test("fun ret() { 1 + 1 + 1 + 1; } fun test() { var = 2; var + ret(); } fun main() { print(test()); } ", "6");
    eval_detail::run_test("fun ret(var) { var; } fun main() { print(ret(123));}", "123");
    eval_detail::run_test("fun create_int() { 1; } fun sum(lhs, rhs) { lhs + rhs; } fun main() { print(sum(1 + 1, create_int())); }", "3");
    eval_detail::run_test("fun main() { var = 1; var = var + 1; print(var); }", "2");
    eval_detail::run_test("fun main() { var = 0; while (var != 10) { var = var + 1; print(var); if (var < 10) { print(\" \"); } } }", "1 2 3 4 5 6 7 8 9 10");
    eval_detail::run_test("fun main() { var = 0; if (var == 0) { println(\"Equal\"); } else { println(\"Different\"); } }", "Equal\n");
    eval_detail::run_test("fun main() { var = 0; if (var != 0) { println(\"Equal\"); } else { println(\"Different\"); } }", "Different\n");
    eval_detail::run_test("fun main() { for (i = 0; i < 10; i = i + 1) { print(i); } }", "0123456789");
    eval_detail::run_test("fun copy(arg) { arg; } fun main() { for (i = 0; i < 10; i = i + 1) { print(copy(i)); } }", "0123456789");
    eval_detail::run_test("fun main() { var = 0  ; print(integer?(var), float?(var)); }", "1 0");
    eval_detail::run_test("fun main() { var = 0.0; print(integer?(var), float?(var)); }", "0 1");
    eval_detail::run_test("fun main() { array = [0, 0, 0]; digit = 1; print(array?(array), array?(digit)); }", "1 0");
    eval_detail::run_test("fun main() { var = \"0\"; print(integer?(var), string?(var)); }", "0 1");
    eval_detail::run_test("fun create_array(a, b, c) { [a, b, c]; } fun main() { array = create_array(1,2,3); print(array?(array)); }", "1");
    eval_detail::run_test("fun create_array(a, b, c) { [a, b, c]; } fun main() { array = create_array(5,6,7); print(array[0]); }", "5");
    eval_detail::run_test("define-type structure(a, b, c); fun main() { structure; }", "");

    eval_detail::run_test(R"__(
        fun sqrt(x) {
            temp = 0.0;
            number = x;
            root = number / 2.0;

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
            sum = 0.0;
            for (i = 0; i < 10000; ++i) {
                sum = sum + sqrt(i);
            }
        }
    )__", "");

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
            print(power(2, 10));

            for (i = 0; i < 1000; ++i) {
                power(2, 1); power(2, 2); power(2, 3); power(2, 4); power(2, 5); power(2, 6); power(2, 7); power(2, power(2, 3));
            }
        }
    )__", "1024");

    eval_detail::run_test(R"__(
        fun _mm256_set_epi8(e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13, e14, e15, e16) {
                           [e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13, e14, e15, e16];
        }

        fun dispatch-argument(argument) {
            if (procedure?(argument)) {
                procedure-arity(argument);
            }
            if (array?(argument)) {
                "Array";
            }
            if (integer?(argument)) {
                "Integer";
            }
        }

        fun main() {
            dispatch-argument(_mm256_set_epi8);
            dispatch-argument([1, 2, 3]);
            dispatch-argument(0);
        }
    )__", "");

    eval_detail::run_test(R"__(
                fun main() {
            array = [
                1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1,
                0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
                1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0,
                0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
                0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
                1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1,
                0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
                1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0,
                0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
                0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
                1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1,
                0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
                1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0,
                0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
                0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
                1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1,
                0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
                1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0,
                0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
                0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0
            ];

            ones = 0; zeros = 0;
            for (tests_count = 0; tests_count < 1000; ++tests_count) {
                for (i = 0; i < 540; ++i) {
                    var = array[i];
                    if (var == 1) {
                        ++ones;
                    } else {
                        ++zeros;
                    }
                }
            }

            print(ones, zeros);
        }
    )__", "240 300");

    eval_detail::expect_error("fun simple() { { var = 2; } var; } fun main() { simple(); }");
    eval_detail::expect_error("fun main() { for (var = 0; var != 10; var = var + 1) { } print(var); }");
    eval_detail::expect_error("fun main() { for (var = 0; var != 10; var = var + 1) { for (var_2 = 0; var_2 != 10; var_2 = var_2 + 1) { print(var); } print(var_2); } }");
    eval_detail::expect_error("fun main() { array = [1, 2, 3]; print(array[0], array[1], array[2], array[3]); }");
    eval_detail::expect_error("fun main() { array = [1, 2, 3]; print(array[0], array[1.44], array[2]); }");
    eval_detail::expect_error("fun main() { array = [1, 2, 3]; index = 1.25; array[index]; }");
    eval_detail::expect_error("fun main() { a = 1; b = \"Text\"; print(a + b); }");
    eval_detail::expect_error("fun main() { a = 1 % 1.5; }");

    std::cout << "Eval tests passed successfully\n";
}

auto create_evaluator(std::string_view program) {
    Lexer lexer(std::istringstream{program.data()});

    Parser parser(lexer.tokenize());

    auto parsed_program = parser.parse();

    SemanticAnalyzer semantic_analyzer(parsed_program);
    semantic_analyzer.analyze();

    Evaluator evaluator(parsed_program);

    return evaluator;
};

void eval_speed_tests()
{
    auto run_test = [](std::string_view description, std::string_view program)
    {
        speed_benchmark(description, 1, [&program]{
            create_evaluator(program).eval();
        });
    };

    run_test("Multiply 1'000'000 * 1'000'000 * 1'000'000 times", R"(
        fun complex() { for (k = 0; k < 1000000; ++k) { for (j = 0; j < 1000000; ++j) { k * j; } } }
        fun main()    { for (i = 0; i < 1000000; ++i) { complex(); } }
    )");
    run_test("Count elements in array 27x20 1'000'000 times", R"(
        fun main() {
            array = [
                1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1,
                0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
                1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0,
                0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
                0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
                1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1,
                0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
                1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0,
                0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
                0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
                1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1,
                0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
                1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0,
                0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
                0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
                1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1,
                0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
                1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0,
                0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
                0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0
            ];

            ones = 0; zeros = 0;
            for (tests_count = 0; tests_count < 1000000; ++tests_count) {
                for (i = 0; i < 540; ++i) {
                    var = array[i];
                    if (var == 1) {
                        ++ones;
                    } else {
                        ++zeros;
                    }
                }
            }

            print(ones, zeros);
        }
)");
}

#endif // WEAK_TESTS_INTERPRETER_HPP
