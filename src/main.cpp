#include <iostream>

#include "../include/tests/lexer_tests.hpp"
#include "../include/tests/parser_tests.hpp"
#include "../include/tests/ast_test.hpp"
#include "../include/tests/semantic_analyzer_tests.hpp"
#include "../include/tests/storage_tests.hpp"
#include "../include/tests/eval_tests.hpp"

std::ostringstream ostream_buffer;
std::ostream& default_stdout = ostream_buffer;

int main()
{
    run_lexer_tests();
    run_expression_tests();
    run_parser_tests();
    run_semantic_analyzer_tests();
    run_storage_tests();
    run_eval_tests();

    std::cout << "All tests passed successfully\n";

    return 0;
}
