cmake_minimum_required(VERSION 3.20)
project(Test)

set(CMAKE_CXX_STANDARD 17)

add_executable(Test main.cpp lexer.hpp lexeme.hpp lexer_test.hpp)
target_link_libraries(Test dl)