#include <iostream>

#include "../include/lexer_test.hpp"

int main()
{
    lexer_number_literal_tests();
    lexer_string_literal_test();
    lexer_symbol_test();
    lexer_operator_test();
    lexer_expression_test();

    std::cout << "All test passed successfully\n";

    return 0;
}
