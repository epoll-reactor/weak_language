#ifndef WEAK_TESTS_INTERPRETER_HPP
#define WEAK_TESTS_INTERPRETER_HPP

#include "../error/eval_error.hpp"
#include "../eval/eval.hpp"
#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"
#include "../semantic/semantic_analyzer.hpp"
#include "../optimizer/optimizer.hpp"

#include "../tests/test_utility.hpp"

#include <sstream>
#include <iostream>

extern std::ostream& default_stdout;

namespace eval_detail {

static int test_counter = 0;

Evaluator create_eval_context(std::string_view program, bool enable_optimizing = false)
{
    Lexer lexer(std::istringstream{program.data()});

    Parser parser(lexer.tokenize());

    auto parsed_program = parser.parse();

    SemanticAnalyzer semantic_analyzer(parsed_program);
    semantic_analyzer.analyze();

    if (enable_optimizing) {
        Optimizer optimizer(parsed_program);
        optimizer.optimize();
    }

    Evaluator evaluator(parsed_program);

    return evaluator;
}

void run_test(std::string_view program, std::string_view expected_output, bool enable_optimizing = true)
{
    std::cout << "Run eval test " << test_counter++ << " => ";

    create_eval_context(program, enable_optimizing).eval();

    try {
        auto& string_stream = dynamic_cast<std::ostringstream&>(default_stdout);

        if (string_stream.str() != expected_output) {
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

    trace_error(program, [evaluator = create_eval_context(program, /*enable_optimizing=*/false), &error]() mutable {
        evaluator.eval();

        /// Will false if exception thrown
        error = false;
    });

    assert(error);
}

auto speed_test(std::string_view description, std::string_view program, bool enable_optimizing = false)
{
    speed_benchmark(description, 1, [evaluator = create_eval_context(program, enable_optimizing)]() mutable {
        evaluator.eval();
    });
};

} // namespace eval_detail

void eval_print_tests()
{
    eval_detail::run_test("fun main() { print(1); }", "1");
    eval_detail::run_test("fun main() { print(\"Text\"); }", "Text");
    eval_detail::run_test("fun main() { variable = \"Text\"; print(variable); }", "Text");
}

void eval_empty_function_tests()
{
    eval_detail::run_test("fun main() {}", "");
    eval_detail::run_test("fun f1() {} fun f2() {} fun f3() {} fun main() { f1(); f2(); f3(); }", "");
}

void eval_arithmetic_tests()
{
    eval_detail::run_test("fun main() { var = 1; var += 1; print(var); }", "2");
    eval_detail::run_test("fun main() { var = 1; var += 1 + 1; print(var); }", "3");
    eval_detail::run_test("fun main() { var = 2; var <<= 10; print(var); }", "2048");
    eval_detail::run_test("fun main() { print(1 + 1.5); }", "2.5");
    eval_detail::run_test("fun main() { print(1.5 + 1); }", "2.5");
    eval_detail::run_test("fun main() { print(1.5 + 1.5); }", "3");
    eval_detail::run_test("fun main() { print(123 % 7); }", "4");
    eval_detail::run_test("fun main() { print(2 << 2, 2 << 9, 2 << 10); }", "8 1024 2048");
    eval_detail::run_test("fun main() { print(1 * 2 * 3 * 4 * 5); }", "120");
    eval_detail::run_test("fun main() { print(++1); }", "2");
    eval_detail::run_test("fun main() { print(--1); }", "0");
    eval_detail::run_test("fun main() { var = 10; print(--var); }", "9");
}

void eval_return_value_tests()
{
    eval_detail::run_test("fun simple() { var = 2; var; } fun main() { print(simple()); }", "2");
    eval_detail::run_test("fun return_string() { \"String\"; } fun main() { print(return_string()); }", "String");
    /// Error             v
    eval_detail::run_test("fun simple() { var = \"Text\"; var; } fun main() { var = simple(); print(var); }", "Text");
    eval_detail::run_test("fun ret() { 1 + 1 + 1 + 1; } fun test() { var = 2; var + ret(); } fun main() { print(test()); } ", "6");
    eval_detail::run_test("fun ret(var) { var; } fun main() { print(ret(123));}", "123");
    eval_detail::run_test("fun create_int() { 1; } fun sum(lhs, rhs) { lhs + rhs; } fun main() { print(sum(1 + 1, create_int())); }", "3");
}

void eval_if_else_tests()
{
    eval_detail::run_test("fun main() { var = 0; if (var == 0) { println(\"Equal\"); } else { println(\"Different\"); } }", "Equal\n");
    eval_detail::run_test("fun main() { var = 0; if (var != 0) { println(\"Equal\"); } else { println(\"Different\"); } }", "Different\n");
}

void eval_for_loop_tests()
{
    eval_detail::run_test("fun main() { for (i = 0; i < 10; ++i) { print(i); } }", "0123456789");
    eval_detail::run_test("fun main() { for (i = 0.0; i <= 2.0; i += 0.5) { print(i, \"\"); } }", "0 0.5 1 1.5 2 ");
    eval_detail::run_test("fun main() { for (i = 0; i < 10; i = i + 1) { print(i); } }", "0123456789");
    eval_detail::run_test("fun copy(arg) { arg; } fun main() { for (i = 0; i < 10; ++i) { print(copy(i)); } }", "0123456789");
}

void eval_while_loop_tests()
{
    eval_detail::run_test("fun main() { i = 0.5; while (i < 5.0) { print(i, \"\"); i += 0.5; } }", "0.5 1 1.5 2 2.5 3 3.5 4 4.5 ");
}

void eval_typecheck_tests()
{
    eval_detail::run_test("fun main() { var = 0  ; print(integer?(var), float?(var)); }", "1 0");
    eval_detail::run_test("fun main() { var = 0.0; print(integer?(var), float?(var)); }", "0 1");
    eval_detail::run_test("fun main() { array = [0, 0, 0]; digit = 1; print(array?(array), array?(digit)); }", "1 0");
    eval_detail::run_test("fun main() { var = \"0\"; print(integer?(var), string?(var)); }", "0 1");
    eval_detail::run_test("fun create_array(a, b, c) { [a, b, c]; } fun main() { array = create_array(1,2,3); print(array?(array)); }", "1");
    eval_detail::run_test("fun create_array(a, b, c) { [a, b, c]; } fun main() { array = create_array(5,6,7); print(array[0]); }", "5");
    eval_detail::run_test("fun compound-procedure(a, b, c, d, e) {} fun main() { print(procedure-arity(compound-procedure)); }", "5");
    eval_detail::run_test("fun compound-procedure () { \"Body\"; } fun main() { print(procedure?(compound-procedure)); }", "1");
}

void eval_user_types_tests()
{
    eval_detail::run_test("define-type structure(a, b, c); fun main() { obj = new structure(1, 2, 3); print(obj); }", "(3, 2, 1)");
    eval_detail::run_test("define-type structure(a, b, c); fun main() { obj = new structure(1, 2, 3); print(obj.a); }", "1");
    eval_detail::expect_error("define-type structure(a, b, c); fun main() { obj = new structure(1); print(obj.a); }");
    eval_detail::expect_error("define-type structure(a, b, c); fun main() { obj = new structure(1, 2, 3); print(obj.field); }");
}

void eval_compound_tests()
{
//    eval_detail::run_test(R"__(
//        fun sqrt(x) {
//            temp = 0.0;
//            number = x;
//            root = number / 2.0;

//            while (root != temp) {
//                temp = root;
//                tmp_value = number / temp;
//                tmp_value += temp;
//                tmp_value /= 2;
//                root = tmp_value;
//            }

//            root;
//        }

//        fun main() {
//            sum = 0.0;
//            for (i = 0; i < 10000; ++i) {
//                sum = sum + sqrt(i);
//            }
//        }
//    )__", "");

    eval_detail::run_test(R"__(
        fun power(num, stage) {
            result = 1;

            while (stage > 0) {
                result *= num;
                --stage;
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
}

void eval_optimizer_reduce_tests()
{
    eval_detail::run_test("fun main() { while (1) {} }", ""); /// Reduced by optimizer
    eval_detail::run_test("fun main() { while (1) { while (1) {} } }", ""); /// Reduced by optimizer
    eval_detail::run_test("fun main() { for (;;) { for (;;) { for (;;) {} } } }", ""); /// Reduced by optimizer
    eval_detail::run_test("fun main() { outer = 1; while (outer) { inner = 1; while (inner) {} } }", ""); /// Reduced by optimizer
    eval_detail::run_test("fun main() { for (i = 0; ; ++i) {} }", ""); /// Reduced by optimizer
    eval_detail::run_test("fun main() { for (i = 0; ; i += 1) {} }", ""); /// Reduced by optimizer
}

void eval_fuzz_tests()
{
    eval_detail::expect_error("fun simple() { var; } fun main() { simple(); }");
    eval_detail::expect_error("fun main() { for (var = 0; var != 10; ++var) { } print(var); }");
    eval_detail::expect_error("fun main() { for (var = 0; var != 10; ++var) { for (var_2 = 0; var_2 != 10; var_2 = var_2 + 1) { print(var); } print(var_2); } }");
    eval_detail::expect_error("fun main() { array = [1, 2, 3]; print(array[0], array[1], array[2], array[3]); }");
    eval_detail::expect_error("fun main() { array = [1, 2, 3]; print(array[0], array[1.44], array[2]); }");
    eval_detail::expect_error("fun main() { array = [1, 2, 3]; index = 1.25; array[index]; }");
    eval_detail::expect_error("fun main() { a = 1; b = \"Text\"; print(a + b); }");
    eval_detail::expect_error("fun main() { a = 1 % 1.5; }");

}

void run_eval_tests()
{
    std::cout << "Running eval tests...\n====\n";

    eval_empty_function_tests();
    eval_arithmetic_tests();
    eval_print_tests();
    eval_return_value_tests();
    eval_if_else_tests();
    eval_for_loop_tests();
    eval_while_loop_tests();
    eval_typecheck_tests();
    eval_user_types_tests();
    eval_compound_tests();
    eval_optimizer_reduce_tests();
    eval_fuzz_tests();

    std::cout << "Eval tests passed successfully\n";
}

void run_eval_speed_tests()
{
    const bool enable_optimizing = true;
    const bool disable_optimizing = false;

    eval_detail::speed_test("Multiply 1'000'000 * 1'000'000 * 1'000'000 times", R"(
        fun complex() { for (k = 0; k < 1000000; ++k) { for (j = 0; j < 1000000; ++j) { k * j; } } }
        fun main()    { for (i = 0; i < 1000000; ++i) { complex(); } }
    )", enable_optimizing);
    eval_detail::speed_test("Count elements in array 27x20 1'000'000 times with for", R"(
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
    )", enable_optimizing);
//    eval_detail::speed_test("Count elements in array 27x20 1'000'000 times with while", R"(
//        fun main() {
//            array = [
//                1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1,
//                0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
//                1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0,
//                0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
//                0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
//                1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1,
//                0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
//                1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0,
//                0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
//                0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
//                1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1,
//                0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
//                1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0,
//                0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
//                0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
//                1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1,
//                0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
//                1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0,
//                0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
//                0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0
//            ];

//            ones = 0; zeros = 0; tests_count = 0;
//            while (tests_count < 1000000) {
//                for (i = 0; i < 540; ++i) {
//                    var = array[i];
//                    if (var == 1) {
//                        ++ones;
//                    } else {
//                        ++zeros;
//                    }
//                }
//                ++tests_count;
//            }

//            print(ones, zeros);
//        }
//    )", enable_optimizing);
    eval_detail::speed_test("Test 6'000'000 unary operations with optimization", R"__(
        fun main() {
            for (i = 0; i < 1000000; ++i) {
                var_1 = ++1;
                var_2 = ++1;
                var_3 = ++1;
                var_4 = ++1;
                var_5 = ++1;
                var_6 = ++1;
            }
        }
    )__", enable_optimizing);
    eval_detail::speed_test("Test 6'000'000 unary operations without optimization", R"__(
        fun main() {
            for (i = 0; i < 1000000; ++i) {
                var_1 = ++1;
                var_2 = ++1;
                var_3 = ++1;
                var_4 = ++1;
                var_5 = ++1;
                var_6 = ++1;
            }
        }
    )__", disable_optimizing);
}

#endif // WEAK_TESTS_INTERPRETER_HPP
