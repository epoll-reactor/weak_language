#include <iostream>

#include "../include/lexer/preprocessor.hpp"

#include "../include/tests/lexer_tests.hpp"
#include "../include/tests/parser_tests.hpp"
#include "../include/tests/ast_test.hpp"
#include "../include/tests/semantic_analyzer_tests.hpp"
#include "../include/tests/storage_tests.hpp"
#include "../include/tests/eval_tests.hpp"

std::ostringstream ostream_buffer;
std::ostream& default_stdout = ostream_buffer;

void eval_file(std::string_view filename)
{
    std::string processed_file = preprocess_file(filename);

    Lexer lexer = LexerBuilder{}
        .operators(test_operators)
        .keywords(test_keywords)
        .input(std::istringstream{processed_file})
        .build();

    Parser parser(lexer.tokenize());
    auto parsed_program = parser.parse();

    SemanticAnalyzer semantic_analyzer(parsed_program);
    semantic_analyzer.analyze();

    Evaluator evaluator(parsed_program);
    evaluator.eval();

    try
    {
        std::cout << dynamic_cast<std::ostringstream&>(default_stdout).str() << '\n';

    } catch (std::bad_cast&) {

    }
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        eval_file(argv[1]);
    }
    else {
        run_lexer_tests();
        run_expression_tests();
        run_parser_tests();
        run_semantic_analyzer_tests();
        run_storage_tests();
        run_eval_tests();
        std::cout << "All tests passed successfully\n";
    }

    return 0;
}
