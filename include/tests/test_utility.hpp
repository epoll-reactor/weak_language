#ifndef TEST_UTILITY_HPP
#define TEST_UTILITY_HPP

#include "../parser/ast.hpp"

#include "../lexer/lexical_error.hpp"
#include "../parser/parse_error.hpp"
#include "../semantic/semantic_error.hpp"
#include "../eval/eval_error.hpp"

extern std::ostream& default_stdout;

void tree_print(std::shared_ptr<ast::Object> ptr)
{
    if (const auto binary = std::dynamic_pointer_cast<ast::Binary>(ptr))
    {
        std::cout << "{";
        tree_print(binary->lhs());
        std::cout << " " << dispatch_lexeme(binary->type()) << " ";
        tree_print(binary->rhs());
        std::cout << "}";
    }
    else if (const auto unary = std::dynamic_pointer_cast<ast::Unary>(ptr)) {
        std::cout << "{";
        std::cout << dispatch_lexeme(unary->type());
        tree_print(unary->operand());
        std::cout << "}";
    }
    else if (auto block = std::dynamic_pointer_cast<ast::Block>(ptr)) {
        std::cout << "{ ";
        for (const auto& statement : block->statements())
        {
            std::cout << " ";
            tree_print(statement);
            std::cout << " ";
        }
        std::cout << "} ";
    }
    else if (auto if_statement = std::dynamic_pointer_cast<ast::If>(ptr)) {
        std::cout << "if (";
        tree_print(if_statement->condition());
        std::cout << ") ";
        tree_print(if_statement->body());
        if (if_statement->else_body())
        {
            std::cout << " else ";
            tree_print(if_statement->else_body());
        }
        std::cout << " ";
    }
    else if (const auto numeric = std::dynamic_pointer_cast<ast::Integer>(ptr)) {
        std::cout << numeric->value();
    }
    else if (const auto string = std::dynamic_pointer_cast<ast::String>(ptr)) {
        std::cout <<  '\"' << string->value() << '\"';
    }
    else if (const auto symbol = std::dynamic_pointer_cast<ast::Symbol>(ptr)) {
        std::cout << symbol->name();
    }
    else if (const auto while_object = std::dynamic_pointer_cast<ast::While>(ptr)) {
        std::cout << "While (";
        tree_print(while_object->exit_condition());
        std::cout << ") ";
        tree_print(while_object->body());
    }
    else if (const auto function_call = std::dynamic_pointer_cast<ast::FunctionCall>(ptr)) {
        std::cout << function_call->name() << "(";
        for (const auto& arg : function_call->arguments())
        {
            tree_print(arg);
            std::cout << " ";
        }
        std::cout << ")";
    }
    else if (const auto function = std::dynamic_pointer_cast<ast::Function>(ptr)) {
        std::cout << function->name() << "(";
        for (const auto& arg : function->arguments())
        {
            tree_print(arg);
            std::cout << " ";
        }
        std::cout << ") ";
        tree_print(function->body());
    }
}

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
