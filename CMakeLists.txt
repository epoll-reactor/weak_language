cmake_minimum_required(VERSION 3.20)
project(WeakLanguage)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER "clang++")
set(OPTIMIZE_FLAGS "-fPIC -O3 -msse3 -march=native -mtune=generic")
set(EXTRA_WARN_FLAGS "-Wall -Wextra -Wcast-qual -Wnon-virtual-dtor -Wreturn-type -Wsign-promo -Wno-unused -Wcast-align -Woverloaded-virtual -Wno-parentheses -Wall -Wextra -pedantic -Wshadow -Wunused-parameter")
set(LEAK_DETECTOR_FLAGS "-fsanitize=leak -fstack-protector")

#add_definitions(-DAST_DEBUG)

set(CMAKE_CXX_FLAGS "${OPTIMISE_FLAGS} ${EXTRA_WARN_FLAGS} ${LEAK_DETECTOR_FLAGS}")

file(GLOB_RECURSE SRC include/*.hpp src/*.cpp)
add_executable(WeakLanguage ${SRC} include/parser/parser.hpp include/parser/ast.hpp include/parser/parse_error.hpp include/tests/test_utility.hpp include/lexer/lexer_builder.hpp include/tests/grammar.hpp include/semantic/semantic_analyzer.hpp include/semantic/semantic_error.hpp include/semantic/storage.hpp include/tests/storage_tests.hpp include/eval/eval.hpp include/tests/eval_tests.hpp include/eval/builtins.hpp include/eval/eval_error.hpp src/eval/builtins.cpp src/eval/eval.cpp src/lexer/lexer.cpp src/lexer/lexer_builder.cpp src/parser/parser.cpp src/semantic/semantic_analyzer.cpp src/lexer/preprocessor.cpp include/lexer/preprocessor.hpp src/ast/root.cpp src/ast/integer.cpp src/ast/string.cpp src/ast/symbol.cpp src/ast/array.cpp src/ast/array_subscript_operator.cpp src/ast/unary.cpp src/ast/binary.cpp src/ast/block.cpp src/ast/while.cpp src/ast/for.cpp src/ast/if.cpp src/ast/function.cpp src/ast/function_call.cpp src/ast/float.cpp src/ast/user_type.cpp)
