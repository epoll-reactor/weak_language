#include <iostream>
#include <cstring>

#include "../include/lexer/preprocessor.hpp"

#include "../include/tests/test_lexer.hpp"
#include "../include/tests/test_semantic.hpp"
#include "../include/tests/test_storage.hpp"
#include "../include/tests/test_interpreter.hpp"

std::ostringstream ostream_buffer;
std::ostream& default_stdout = ostream_buffer;

void eval(std::string_view program)
{
    trace_error("", [&program]{

        Lexer lexer(std::istringstream{program.data()});

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

float run_tests()
{
    auto start = std::chrono::high_resolution_clock::now();

    run_lexer_tests();
    run_semantic_analyzer_tests();
    run_storage_tests();
    run_eval_tests();
    eval_speed_tests();

    auto time_spent = std::chrono::high_resolution_clock::now() - start;
    return std::chrono::duration_cast<std::chrono::duration<float>>(time_spent).count();
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
            #define tests_to_run 1
            std::array<float, tests_to_run> times;

            for (int i = 0; i < tests_to_run; ++i)
                times[i] = run_tests();

            for (int i = 0; i < tests_to_run; ++i)
                std::cout << "Test " << i << ": " << times[i] << " s.\n";
            #undef tests_to_run
        }
        else {
            eval_file(argv[1]);
        }
    }

    return 0;
}
