cmake_minimum_required(VERSION 3.20)
project(WeakLanguage)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_VERBOSE_MAKEFILE 0)

set(common_flags "-fPIC -Wall -Wextra -pedantic -Wshadow -Wunused-parameter")

set(find_leaks_flag      0)
set(extra_warnings_flag  1)
set(optimize_flag        1)

if (${find_leaks_flag})
    set(find_leaks_flags "-fsanitize=leak -fstack-protector")
else()
    set(find_leaks_flags "")
endif()

if (${extra_warnings_flag})
    set(extra_warnings_flags "-Werror -Wcast-qual -Wnon-virtual-dtor -Wreturn-type -Wsign-promo -Wno-unused -Wcast-align -Woverloaded-virtual -Wno-parentheses -fstrict-aliasing")
else()
    set(extra_warnings_flags "")
endif()

if (${optimize_flag})
    set(optimize_flags "-O3 -msse2 -march=native -ftree-vectorize -flto -ffast-math -fearly-inlining -funsafe-math-optimizations -funroll-all-loops")
else()
    set(optimize_flags "")
endif()

set(CMAKE_CXX_FLAGS "${common_flags} ${find_leaks_flags} ${extra_warnings_flags} ${extra_warnings_flags} ${optimize_flags}")


file(GLOB_RECURSE SRC include/*.hpp src/*.cpp)
add_executable(WeakLanguage ${SRC} include/eval/implementation/binary.hpp include/eval/implementation/unary.hpp src/eval/implementation/unary.cpp src/eval/implementation/binary.cpp src/storage/storage.cpp include/error/common_error.hpp include/format.hpp include/tests/test_format.hpp src/ast/type_object.cpp src/ast/type_field_operator.cpp src/ast/type_creator.cpp)
