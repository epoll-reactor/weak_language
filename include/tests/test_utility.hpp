#ifndef TEST_UTILITY_HPP
#define TEST_UTILITY_HPP

#include "../parser/expression.hpp"


void tree_print(std::shared_ptr<expression::Object> ptr)
{
    if (const auto binary = std::dynamic_pointer_cast<expression::Binary>(ptr))
    {
        std::cout << "{";
        tree_print(binary->lhs());
        std::cout << " " << dispatch_lexeme(binary->type()) << " ";
        tree_print(binary->rhs());
        std::cout << "}";
    }
    else if (const auto unary = std::dynamic_pointer_cast<expression::Unary>(ptr)) {
        std::cout << "{";
        std::cout << dispatch_lexeme(unary->type());
        tree_print(unary->operand());
        std::cout << "}";
    }
    else if (auto block = std::dynamic_pointer_cast<expression::Block>(ptr)) {
        std::cout << "{ ";
        for (const auto& statement : block->statements())
        {
            std::cout << " ";
            tree_print(statement);
            std::cout << " ";
        }
        std::cout << "} ";
    }
    else if (auto if_statement = std::dynamic_pointer_cast<expression::If>(ptr)) {
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
    else if (const auto numeric = std::dynamic_pointer_cast<expression::Number>(ptr)) {
        std::cout << numeric->value();
    }
    else if (const auto string = std::dynamic_pointer_cast<expression::String>(ptr)) {
        std::cout <<  '\"' << string->value() << '\"';
    }
    else if (const auto symbol = std::dynamic_pointer_cast<expression::Symbol>(ptr)) {
        std::cout << symbol->name();
    }
    else if (const auto while_object = std::dynamic_pointer_cast<expression::While>(ptr)) {
        std::cout << "While (";
        tree_print(while_object->exit_condition());
        std::cout << ") ";
        tree_print(while_object->body());
    }
}

double tree_sum(std::shared_ptr<expression::Object> ptr)
{
    if (const auto numeric = std::dynamic_pointer_cast<expression::Number>(ptr))
    {
        return numeric->value();
    }

    if (const auto additive = std::dynamic_pointer_cast<expression::Binary>(ptr))
    {
        return tree_sum(additive->lhs()) + tree_sum(additive->rhs());
    }
    else {
        return 0;
    }
}

#endif // TEST_UTILITY_HPP
