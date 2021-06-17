#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP

#include "../parser/ast.hpp"
#include "../semantic/semantic_error.hpp"


class SemanticAnalyzer
{
public:
    SemanticAnalyzer(std::shared_ptr<ast::RootObject> input)
        : m_input(std::move(input->get()))
    { }

    void analyze()
    {
        for (const auto& expression : m_input)
        {
            analyze_statement(expression);
        }
    }

private:
    void analyze_statement(std::shared_ptr<ast::Object> statement)
    {
        if (auto if_statement = std::dynamic_pointer_cast<ast::If>(statement))
        {
            analyze_if_statement(if_statement);
        }
        else if (auto while_statement = std::dynamic_pointer_cast<ast::While>(statement)) {

            analyze_while_statement(while_statement);
        }
        else if (auto binary_statement = std::dynamic_pointer_cast<ast::Binary>(statement)) {

            if (binary_statement->type() == lexeme_t::assign
            ||  binary_statement->type() == lexeme_t::plus_assign
            ||  binary_statement->type() == lexeme_t::minus_assign
            ||  binary_statement->type() == lexeme_t::star_assign
            ||  binary_statement->type() == lexeme_t::slash_assign)
            {
                analyze_assign_statement(binary_statement);
            }
            else {
                analyze_binary_statement(binary_statement);
            }
        }
    }

    void analyze_assign_statement(std::shared_ptr<ast::Binary> statement)
    {
        if (!std::dynamic_pointer_cast<ast::Symbol>(statement->lhs()))
        {
            throw SemanticError("Expression is not assignable");
        }

        if (!std::dynamic_pointer_cast<ast::Symbol>(statement->rhs()) &&
            !std::dynamic_pointer_cast<ast::Number>(statement->rhs()) &&
            !std::dynamic_pointer_cast<ast::String>(statement->rhs()) &&
            !std::dynamic_pointer_cast<ast::Binary>(statement->rhs()))
        {
            throw SemanticError("Expression is not assignable");
        }

        if (auto binary = std::dynamic_pointer_cast<ast::Binary>(statement->rhs()))
        {
            analyze_binary_statement(binary);
        }
    }

    void analyze_binary_statement(std::shared_ptr<ast::Binary> statement)
    {
        switch (statement->type())
        {
            case lexeme_t::plus:
            case lexeme_t::minus:
            case lexeme_t::star:
            case lexeme_t::slash:
            case lexeme_t::equal:
                break;

            default:
                throw SemanticError("Incorrect binary expression operator");
        }

        if (auto lhs = std::dynamic_pointer_cast<ast::Binary>(statement->lhs()))
        {
            analyze_binary_statement(lhs);
        }
        else if (auto rhs = std::dynamic_pointer_cast<ast::Binary>(statement->rhs())) {

            analyze_binary_statement(rhs);
        }
    }

    void analyze_if_statement(std::shared_ptr<ast::If> if_statement)
    {
        if (!to_bool_convertible(if_statement->condition()))
        {
            throw SemanticError("If condition requires convertible to bool expression");
        }

        auto if_body = std::dynamic_pointer_cast<ast::Block>(if_statement->body());
        auto else_body = std::dynamic_pointer_cast<ast::Block>(if_statement->else_body());

        for (const auto& if_instruction : if_body->statements())
        {
            analyze_statement(if_instruction);
        }

        if (else_body)
        {
            for (const auto& if_instruction : else_body->statements())
            {
                analyze_statement(if_instruction);
            }
        }
    }

    void analyze_while_statement(std::shared_ptr<ast::While> while_statement)
    {
        if (!to_bool_convertible(while_statement->exit_condition()))
        {
            throw SemanticError("While condition requires convertible to bool expression");
        }

        auto body = std::dynamic_pointer_cast<ast::Block>(while_statement->body());

        for (const auto& statement : body->statements())
        {
            analyze_statement(statement);
        }
    }

    bool to_bool_convertible(std::shared_ptr<ast::Object> statement)
    {
        if (std::dynamic_pointer_cast<ast::Number>(statement))
        {
            return true;
        }
        else if (std::dynamic_pointer_cast<ast::Symbol>(statement)) {

            return true;
        }
        else if (auto binary_expression = std::dynamic_pointer_cast<ast::Binary>(statement)) {

            try
            {
                analyze_binary_statement(binary_expression);

            } catch (SemanticError&) {

                return false;
            }

            return true;
        }
        else {
            return false;
        }
    }

    std::vector<std::shared_ptr<ast::Object>> m_input;
};

#endif // SEMANTIC_ANALYZER_HPP