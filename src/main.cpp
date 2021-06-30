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

void eval(std::string_view program)
{
    trace_error("", [&program]{

        Lexer lexer = LexerBuilder{}
            .operators(test_operators)
            .keywords(test_keywords)
            .input(std::istringstream{program.data()})
            .build();

        Parser parser(lexer.tokenize());
        auto parsed_program = parser.parse();

        SemanticAnalyzer semantic_analyzer(parsed_program);
        semantic_analyzer.analyze();

        Evaluator evaluator(parsed_program);
        evaluator.eval();

        try
        {
            auto& ostream = dynamic_cast<std::ostringstream&>(default_stdout);
            std::cout << ostream.str() << '\n';
            ostream.str("");

        } catch (std::bad_cast&) {

        }

        default_stdout.clear();
    });
}

void eval_file(std::string_view filename)
{
    std::string processed_file = preprocess_file(filename);

    eval(processed_file);
}

[[noreturn]] void run_repr()
{
    while (true)
    {
        std::cout << ">>> ";

        std::string program;
        std::getline(std::cin, program);

        eval(program);
    }
}

void run_tests()
{
    run_lexer_tests();
    run_ast_tests();
    run_parser_tests();
    run_semantic_analyzer_tests();
    run_storage_tests();
    run_eval_tests();
    std::cout << "All tests passed successfully\n";
}

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
         run_repr();
    }
    else if (argc == 2)
    {
        if (strcmp(argv[1], "test") == 0)
        {
            run_tests();
        }
        else {
            eval_file(argv[1]);
        }
    }

    return 0;
}
