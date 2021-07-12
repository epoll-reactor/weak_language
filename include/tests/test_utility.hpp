#ifndef TEST_UTILITY_HPP
#define TEST_UTILITY_HPP

#include <chrono>
#include <iostream>

#include "../ast/ast.hpp"

#include "../error/lexical_error.hpp"
#include "../error/parse_error.hpp"
#include "../error/semantic_error.hpp"
#include "../error/eval_error.hpp"

extern std::ostream& default_stdout;

template <typename Fun>
void trace_error(std::string_view program, Fun&& fn)
{
    try
    {
        fn();

    } catch (LexicalError& lexical_error) {

        std::cout << "While analyzing:\n\t" << program << "\nLexical error processed:\n\t" << lexical_error.what() << "\n\n";
        goto clear_stdout;

    } catch (ParseError& parse_error) {

        std::cout << "While analyzing:\n\t" << program << "\nParse error processed:\n\t" << parse_error.what() << "\n\n";
        goto clear_stdout;

    } catch (SemanticError& semantic_error) {

        std::cout << "While analyzing:\n\t" << program << "\nSemantic error processed\n\t" << semantic_error.what() << "\n\n";
        goto clear_stdout;

    } catch (EvalError& eval_error) {

        std::cout << "While analyzing:\n\t" << program << "\nEval error processed\n\t" << eval_error.what() << "\n\n";
        goto clear_stdout;
    }

clear_stdout:
    try
    {
        auto& string_stream = dynamic_cast<std::ostringstream&>(default_stdout);

        string_stream.str("");

    } catch (std::bad_cast&) { }

    default_stdout.clear();
}

template <class Function, class... Args>
void speed_benchmark(std::string_view label, uint64_t iterations, Function function, Args&&... args)
{
    std::cout << std::setw(50) << label << "\t: " << iterations << " iteration(s): ";
    auto start = std::chrono::high_resolution_clock::now();
    while (iterations-- > 0)
        function(std::forward<Args>(args)...);
    auto time_spent = std::chrono::high_resolution_clock::now() - start;
    std::cout << std::chrono::duration_cast<std::chrono::duration<float>>(time_spent).count() << " s. (";
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(time_spent).count() << " ms.)" << std::endl;
}

template <class Function, class... Args>
void speed_benchmark(uint64_t iterations, Function function, Args&&... args)
{
    std::string info = std::to_string(iterations) + " iteration(s): ";
    auto start = std::chrono::high_resolution_clock::now();
    while (iterations-- > 0)
        function(std::forward<Args>(args)...);
    auto time_spent = std::chrono::high_resolution_clock::now() - start;
    info += std::to_string(std::chrono::duration_cast<std::chrono::duration<float>>(time_spent).count());
    info += " s., (";
    info += std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(time_spent).count());
    info += " ms.)";
    std::cout << info << std::endl;
}

#endif // TEST_UTILITY_HPP
