#ifndef WEAK_TESTS_EVAL_HPP
#define WEAK_TESTS_EVAL_HPP

#include "../error/eval_error.hpp"
#include "../eval/eval.hpp"
#include "../lexer/lexer.hpp"
#include "../optimizer/optimizer.hpp"
#include "../parser/parser.hpp"
#include "../semantic/semantic_analyzer.hpp"
#include "../tests/test_utility.hpp"

#include <iostream>
#include <sstream>

extern std::ostream& default_stdout;

namespace eval_detail {

static int test_counter = 0;

Evaluator create_eval_context(std::string_view program, bool enable_optimizing = false) noexcept(false) {
  Lexer lexer(std::istringstream{program.data()});
  Parser parser(lexer.tokenize());
  auto parsed_program = parser.parse();
  SemanticAnalyzer semantic_analyzer(parsed_program);
  semantic_analyzer.analyze();
  if (enable_optimizing) {
    Optimizer optimizer(parsed_program);
    optimizer.optimize();
  }
  return Evaluator(parsed_program);
}

void run_test(std::string_view program, std::string_view expected_output, bool enable_optimizing = true) noexcept(false) {
  std::cout << "Run eval test " << test_counter++ << " => ";
  create_eval_context(program, enable_optimizing).eval();
  try {
    auto& stream = dynamic_cast<std::ostringstream&>(default_stdout);
    if (stream.str() != expected_output) {
      std::cerr << "eval error: for " << program << "\n\tgot [" << stream.str() << "], expected [" << expected_output << "]\n";
      exit(-1);
    }
    stream.str("");
  } catch (std::bad_cast&) {}

  default_stdout.clear();
  std::cout << "OK\n";
}

//#define ENABLE_FUZZING
void expect_error(std::string_view program) {
#ifndef ENABLE_FUZZING
  (void)program;
#else
  std::cout << "Run fuzz test " << test_counter++ << " => ";
  bool error = true;
  trace_error(program, [evaluator = create_eval_context(program, /*enable_optimizing=*/false), &error]() mutable {
    evaluator.eval();
    /// Will false if exception thrown
    error = false;
  });
  assert(error);
#endif// ENABLE_FUZZING
}
#undef ENABLE_FUZZING

auto speed_test(std::string_view description, std::string_view program, bool enable_optimizing = false) {
  speed_benchmark(description, 1, [evaluator = create_eval_context(program, enable_optimizing)]() mutable {
    evaluator.eval();
  });
};

}// namespace eval_detail

void eval_print_tests() {
  eval_detail::run_test("lambda main() { print(1); }", "1");
  eval_detail::run_test("lambda main() { print(\"Text\"); }", "Text");
  eval_detail::run_test("lambda main() { variable = \"Text\"; print(variable); }", "Text");
}

void eval_empty_lambda_tests() {
  eval_detail::run_test("lambda main() {}", "");
  eval_detail::run_test("lambda f1() {} lambda f2() {} lambda f3() {} lambda main() { f1(); f2(); f3(); }", "");
}

void eval_inner_lambdas_tests() {
  eval_detail::run_test("lambda main() { lambda inner() { 1; } print(inner()); }", "1");
  eval_detail::expect_error("lambda main() { lambda inner_1() { lambda inner_2() {} } inner_2(); }");
}

void eval_arithmetic_tests() {
  eval_detail::run_test("lambda main() { var = 1; var += 1; print(var); }", "2");
  eval_detail::run_test("lambda main() { var = 1; var += 1 + 1; print(var); }", "3");
  eval_detail::run_test("lambda main() { var = 2; var <<= 10; print(var); }", "2048");
  eval_detail::run_test("lambda main() { print(1 + 1.5); }", "2.5");
  eval_detail::run_test("lambda main() { print(1.5 + 1); }", "2.5");
  eval_detail::run_test("lambda main() { print(1.5 + 1.5); }", "3");
  eval_detail::run_test("lambda main() { print(123 % 7); }", "4");
  //  eval_detail::run_test("lambda main() { print(2 * (2 + 2)); }", "8");
  //  eval_detail::run_test("lambda main() { print((1 + 1) * (1 + 1)); }", "4");
  eval_detail::run_test("lambda main() { print(2 << 2, 2 << 9, 2 << 10); }", "8 1024 2048");
  eval_detail::run_test("lambda main() { print(1 * 2 * 3 * 4 * 5); }", "120");
  eval_detail::run_test("lambda main() { print(++1); }", "2");
  eval_detail::run_test("lambda main() { print(--1); }", "0");
  eval_detail::run_test("lambda main() { var = 10; print(--var); }", "9");
  eval_detail::run_test("lambda main() { _1 = 1; _2 = 2; _3 = 3; print(_1 + 1 + _2 + 2 + _3 + 3); }", "12");
}

void eval_return_value_tests() {
  eval_detail::run_test("lambda simple() { var = 2; var; } lambda main() { print(simple()); }", "2");
  eval_detail::run_test("lambda return_string() { \"String\"; } lambda main() { print(return_string()); }", "String");
  eval_detail::run_test("lambda simple() { var = \"Text\"; var; } lambda main() { print(simple()); }", "Text");
  eval_detail::run_test("lambda ret() { 1 + 1 + 1 + 1; } lambda test() { var = 2; var + ret(); } lambda main() { print(test()); } ", "6");
  eval_detail::run_test("lambda ret(var) { var; } lambda main() { print(ret(123));}", "123");
  eval_detail::run_test("lambda create_int() { 1; } lambda sum(lhs, rhs) { lhs + rhs; } lambda main() { print(sum(1 + 1, create_int())); }", "3");
  eval_detail::run_test("lambda return-different(x) { value = 0; if (x == 0) { value = 0; } else { value = \"\xFF\"; } value; } lambda main() { print(return-different(0), return-different(1)); }", "0 \xFF");
}

void eval_if_else_tests() {
  eval_detail::run_test("lambda main() { var = 0; if (var == 0) { print(\"Equal\"); } else { print(\"Different\"); } }", "Equal");
  eval_detail::run_test("lambda main() { var = 0; if (var != 0) { print(\"Equal\"); } else { print(\"Different\"); } }", "Different");
}

void eval_for_loop_tests() {
  eval_detail::run_test("lambda main() { for (i = 0; i < 10; ++i) { print(i); } }", "0123456789");
  eval_detail::run_test("lambda main() { for (i = 0.0; i <= 2.0; i += 0.5) { print(i, \"\"); } }", "0 0.5 1 1.5 2 ");
  eval_detail::run_test("lambda main() { for (i = 0; i < 10; i = i + 1) { print(i); } }", "0123456789");
  eval_detail::run_test("lambda main() { for (i = 0; i < 1 + 2 + 3 + 4; ++i) { print(i); } }", "0123456789");
  eval_detail::run_test("lambda main() { result = 1; for (i = 0; i < 5; ++i) { result = result + 1; } print(result); }", "6");
  eval_detail::run_test("lambda copy(arg) { arg; } lambda main() { for (i = 0; i < 10; ++i) { print(copy(i)); } }", "0123456789");
}

void eval_while_loop_tests() {
  eval_detail::run_test("lambda main() { i = 0.5; while (i < 5.0) { print(i, \"\"); i += 0.5; } }", "0.5 1 1.5 2 2.5 3 3.5 4 4.5 ");
}

void eval_array_access_tests() {
  eval_detail::run_test("lambda main() { array = [1, 2, 3]; print(array-get(array, 2)); }", "3");
  eval_detail::run_test("lambda main() { array = [1.1, 2.2, 3.3]; print(array-get(array, 1)); }", "2.2");
  eval_detail::run_test("lambda main() { array = [\"Text1\", \"Text2\", \"Text3\"]; print(array-get(array, 2)); }", "Text3");

  eval_detail::run_test("lambda main() { array = [1, 2, 3]; array-set(array, 0, 3); print(array-get(array, 0)); }", "3");
  eval_detail::run_test("lambda main() { array = [\"Text1\", \"Text2\", \"Text3\"]; array-set(array, 0, \"Text123\"); print(array-get(array, 0)); }", "Text123");
}

void eval_typecheck_tests() {
  eval_detail::run_test("lambda main() { var = 0  ; print(integer?(var), float?(var)); }", "1 0");
  eval_detail::run_test("lambda main() { var = 0.0; print(integer?(var), float?(var)); }", "0 1");
  eval_detail::run_test("lambda main() { array = [0, 0, 0]; digit = 1; print(array?(array), array?(digit)); }", "1 0");
  eval_detail::run_test("lambda main() { var = \"0\"; print(integer?(var), string?(var)); }", "0 1");
  eval_detail::run_test("lambda create_array(a, b, c) { [a, b, c]; } lambda main() { array = create_array(1,2,3); print(array?(array)); }", "1");
  eval_detail::run_test("lambda create_array(a, b, c) { [a, b, c]; } lambda main() { array = create_array(5,6,7); print(array-get(array, 0)); }", "5");
  eval_detail::run_test("lambda compound-procedure(a, b, c, d, e) {} lambda main() { print(procedure-arity(compound-procedure)); }", "5");
  eval_detail::run_test("lambda compound-procedure () { \"Body\"; } lambda main() { print(procedure?(compound-procedure)); }", "1");
}

void eval_user_types_tests() {
  //  eval_detail::run_test("define-type structure(a, b, c); lambda main() { obj = new structure(1, 2, 3); print(obj); }", "(1, 2, 3)");
  //  eval_detail::run_test("define-type structure(a, b, c); lambda main() { obj = new structure(1, 2, 3); print(obj.a); }", "1");
  //  eval_detail::run_test("define-type structure(a); lambda get_field(struct) { struct.a; } lambda main() { obj = new structure(1); print(get_field(obj)); }", "1");
  //  eval_detail::expect_error("define-type structure(a, b, c); lambda main() { obj = new structure(1); print(obj.a); }");
  //  eval_detail::expect_error("define-type structure(a, b, c); lambda main() { obj = new structure(1, 2, 3); print(obj.field); }");
}

void eval_simple_algorithms() {
  eval_detail::run_test(R"__(
    lambda factorial(x) {
      result = 0;
      if (x <= 1) {
        result = 1;
      } else {
        result = x * factorial(x - 1);
      }
      result;
    }

    lambda main() {
      print(factorial(12));
    }
  )__",
                        "479001600");
  eval_detail::run_test(R"__(
    lambda sqrt(x) {
      temp = 0.0;
      number = x;
      root = number / 2.0;

      while (root != temp) {
        temp = root;
        tmp_value = number / temp;
        tmp_value += temp;
        tmp_value /= 2.0;
        root = tmp_value;
      }
      root;
    }
    lambda main() {
      print(sqrt(81));
    }
  )__",
                        "9");
  eval_detail::run_test(R"__(
    lambda power(num, stage) {
      result = 1;
      while (stage > 0) {
        result *= num;
        --stage;
      }
      result;
    }
    lambda main() {
      print(power(2, 10));
    }
  )__",
                        "1024");
  eval_detail::run_test(R"__(
    lambda main() {
      array = [0, 1];
      for (i = 0; i < 2; ++i) {
        array-set(array, i, 10);
      }
      print(array);
    }
  )__",
                        "[10, 10]");
}

void eval_compound_tests() {
  eval_detail::run_test(R"__(
        lambda _mm256_set_epi8(e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13, e14, e15, e16) {
                           [e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13, e14, e15, e16];
        }

        lambda dispatch-argument(argument) {
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

        lambda main() {
            dispatch-argument(_mm256_set_epi8);
            dispatch-argument([1, 2, 3]);
            dispatch-argument(0);
        }
    )__",
                        "");

  eval_detail::run_test(R"__(
        lambda main() {
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
                    var = array-get(array, i);
                    if (var == 1) {
                        ++ones;
                    } else {
                        ++zeros;
                    }
                }
            }

            print(ones, zeros);
        }
    )__",
                        "240 300");
}

void eval_optimizer_reduce_tests() {
  eval_detail::run_test("lambda main() { while (1) {} }", "");
  eval_detail::run_test("lambda main() { while (1) { while (1) {} } }", "");
  eval_detail::run_test("lambda main() { for (;;) { for (;;) { for (;;) {} } } }", "");
  eval_detail::run_test("lambda main() { outer = 1; while (outer) { inner = 1; while (inner) {} } }", "");
  eval_detail::run_test("lambda main() { for (i = 0; ; ++i) {} }", "");
  eval_detail::run_test("lambda main() { for (i = 0; ; i += 1) {} }", "");
}

void eval_fuzz_tests() {
  eval_detail::expect_error("lambda simple() { var; } lambda main() { simple(); }");
  eval_detail::expect_error("lambda main() { for (var = 0; var != 10; ++var) { } print(var); }");
  eval_detail::expect_error("lambda main() { for (var = 0; var != 10; ++var) { for (var_2 = 0; var_2 != 10; var_2 = var_2 + 1) { print(var); } print(var_2); } }");
  eval_detail::expect_error("lambda main() { array = [1, 2, 3]; print(array[0], array[1], array[2], array[3]); }");
  eval_detail::expect_error("lambda main() { array = [1, 2, 3]; print(array[0], array[1.44], array[2]); }");
  eval_detail::expect_error("lambda main() { array = [1, 2, 3]; index = 1.25; array[index]; }");
  eval_detail::expect_error("lambda main() { a = 1; b = \"Text\"; print(a + b); }");
  eval_detail::expect_error("lambda main() { a = 1 % 1.5; }");
}

void run_eval_tests() {
  std::cout << "Running eval tests...\n====\n";

  eval_empty_lambda_tests();
  eval_inner_lambdas_tests();
  eval_arithmetic_tests();
  eval_print_tests();
  eval_return_value_tests();
  eval_if_else_tests();
  eval_for_loop_tests();
  eval_while_loop_tests();
  eval_array_access_tests();
  eval_simple_algorithms();
  eval_typecheck_tests();
  eval_user_types_tests();
  eval_compound_tests();
  eval_optimizer_reduce_tests();
  eval_fuzz_tests();

  std::cout << "Eval tests passed successfully\n";
}

void run_eval_speed_tests() {
  const bool enable_optimizing = true;
  const bool disable_optimizing = false;

  eval_detail::speed_test("Multiply 1'000'000 * 1'000'000 * 1'000'000 times", R"(
        lambda complex() { for (k = 0; k < 1000000; ++k) { for (j = 0; j < 1000000; ++j) { k * j; } } }
        lambda main()    { for (i = 0; i < 1000000; ++i) { complex(); } }
    )",
                          enable_optimizing);
  eval_detail::speed_test("Count elements in array 27x20 1'000'000 times with for", R"(
        lambda main() {
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
                    var = array-get(array, i);
                    if (var == 1) {
                        ++ones;
                    } else {
                        ++zeros;
                    }
                }
            }

            print(ones, zeros);
        }
    )",
                          enable_optimizing);
  eval_detail::speed_test("Test 10'000'000 unary operations with optimization", R"__(
        lambda main() {
            for (i = 0; i < 1000000; ++i) {
                var_1 = ++1;
                var_2 = ++1;
                var_3 = ++1;
                var_4 = ++1;
                var_5 = ++1;
                var_6 = ++1;
                var_7 = ++1;
                var_8 = ++1;
                var_9 = ++1;
                var_10 = ++1;
            }
        }
    )__",
                          enable_optimizing);
  eval_detail::speed_test("Test 10'000'000 unary operations without optimization", R"__(
        lambda main() {
            for (i = 0; i < 1000000; ++i) {
                var_1 = ++1;
                var_2 = ++1;
                var_3 = ++1;
                var_4 = ++1;
                var_5 = ++1;
                var_6 = ++1;
                var_7 = ++1;
                var_8 = ++1;
                var_9 = ++1;
                var_10 = ++1;
            }
        }
    )__",
                          disable_optimizing);
}

#endif// WEAK_TESTS_EVAL_HPP
