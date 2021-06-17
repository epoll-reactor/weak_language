#include <iostream>

#include "../include/tests/lexer_tests.hpp"
#include "../include/tests/parser_tests.hpp"
#include "../include/tests/expression_test.hpp"
#include "../include/tests/semantic_analyzer_tests.hpp"

int main()
{
    run_lexer_tests();
    run_expression_tests();
    run_parser_tests();
    run_semantic_analyzer_tests();

    std::cout << "All tests passed successfully\n";

    return 0;
}
