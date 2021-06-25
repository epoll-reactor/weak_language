#ifndef EVAL_HPP
#define EVAL_HPP

#include "../parser/ast.hpp"
#include "../semantic/storage.hpp"
#include "../expressions/builtins.hpp"


class Evaluator
{
public:
    Evaluator(std::shared_ptr<ast::RootObject> program)
        : m_expressions(program->get())
    { }

    void eval()
    {
        for (auto expr : m_expressions)
        {
            if (auto function = std::dynamic_pointer_cast<ast::Function>(expr))
            {
                m_storage.push(function->name(), function);
            }
            else {
                throw EvalError("Only functions as global objects supported");
            }
        }

        call_function("main", {});
    }

private:
    std::shared_ptr<ast::Object> call_function(std::string_view name, std::vector<std::shared_ptr<ast::Object>> evaluated_args)
    {
        if (auto stored_function = std::dynamic_pointer_cast<ast::Function>(m_storage.lookup(name)))
        {
            if (stored_function->arguments().size() != evaluated_args.size())
                throw EvalError("Wrong arguments size");

            std::shared_ptr<ast::Object> last_statement;
            /// Load function arguments to local scope
            m_storage.scope_begin();

            for (std::size_t i = 0; i < evaluated_args.size(); i++)
                m_storage.push(std::dynamic_pointer_cast<ast::Symbol>(stored_function->arguments()[i])->name(), evaluated_args[i]);

            for (const auto& arg : stored_function->body()->statements())
                 last_statement = eval_expression(arg);

            m_storage.scope_end();

            if (std::dynamic_pointer_cast<ast::Number>(last_statement)
            ||  std::dynamic_pointer_cast<ast::String>(last_statement))
            {
                return last_statement;
            }
            else {
                return {};
            }
        }
        else {
            throw EvalError("Try to call not a function");
        }
    }

    std::shared_ptr<ast::Object> eval_binary(std::shared_ptr<ast::Binary> binary)
    {
        auto binary_impl = [&](lexeme_t op_type, std::shared_ptr<ast::Object> lhs, std::shared_ptr<ast::Object> rhs) {

            auto left_num = std::dynamic_pointer_cast<ast::Number>(eval_expression(binary->lhs()));
            auto right_num = std::dynamic_pointer_cast<ast::Number>(eval_expression(binary->rhs()));

            if (left_num && right_num)
                switch (op_type)
                {
                    case lexeme_t::plus:
                        return std::make_shared<ast::Number>(left_num->value() + right_num->value());

                    case lexeme_t::minus:
                        return std::make_shared<ast::Number>(left_num->value() - right_num->value());

                    case lexeme_t::star:
                        return std::make_shared<ast::Number>(left_num->value() * right_num->value());

                    case lexeme_t::slash:
                        return std::make_shared<ast::Number>(left_num->value() / right_num->value());
                    /// WTF
                    case lexeme_t::remainder:
                        return std::make_shared<ast::Number>((int)(left_num->value()) % (int)(right_num->value()));

                    case lexeme_t::eq:
                        return std::make_shared<ast::Number>(left_num->value() == right_num->value());

                    case lexeme_t::neq:
                        return std::make_shared<ast::Number>(left_num->value() != right_num->value());

                    case lexeme_t::ge:
                        return std::make_shared<ast::Number>(left_num->value() >= right_num->value());

                    case lexeme_t::gt:
                        return std::make_shared<ast::Number>(left_num->value() > right_num->value());

                    case lexeme_t::le:
                        return std::make_shared<ast::Number>(left_num->value() <= right_num->value());

                    case lexeme_t::lt:
                        return std::make_shared<ast::Number>(left_num->value() < right_num->value());

                    default:
                        throw EvalError("Unsupported binary operator");
                }
            else
                throw EvalError("Unsupported binary operands");
        };

        if (binary->type() == lexeme_t::assign)
        {
            m_storage.overwrite(std::dynamic_pointer_cast<ast::Symbol>(binary->lhs())->name(), eval_expression(binary->rhs()));

            return binary;
        }
        else {
            return binary_impl(binary->type(), binary->lhs(), binary->rhs());
        }
    }

    void eval_while(std::shared_ptr<ast::While> while_stmt)
    {
        auto exit_condition = eval_expression(while_stmt->exit_condition());

        if (auto boolean_exit_condition = std::dynamic_pointer_cast<ast::Number>(exit_condition))
        {
            while (boolean_exit_condition->value())
            {
                eval_expression(while_stmt->body());

                boolean_exit_condition = std::dynamic_pointer_cast<ast::Number>(eval_expression(while_stmt->exit_condition()));
            }
        }
        else {
            throw EvalError("While requires bool-convertible exit condition");
        }
    }

    void eval_if(std::shared_ptr<ast::If> if_stmt)
    {
        auto if_condition = eval_expression(if_stmt->condition());

        if (std::dynamic_pointer_cast<ast::Number>(if_condition)->value())
        {
            eval_expression(if_stmt->body());
        }
        else if (auto else_body = if_stmt->else_body()) {

            eval_expression(else_body);
        }
    }

    std::shared_ptr<ast::Object> eval_expression(std::shared_ptr<ast::Object> expression)
    {
        if (const auto function_call = std::dynamic_pointer_cast<ast::FunctionCall>(expression))
        {
            std::vector<std::shared_ptr<ast::Object>> evaluated_arguments;

            for (const auto& arg : function_call->arguments())
            {
                if (auto var = std::dynamic_pointer_cast<ast::Symbol>(arg))
                {
                    evaluated_arguments.emplace_back(m_storage.lookup(var->name()));
                    continue;
                }

                evaluated_arguments.emplace_back(eval_expression(arg));
            }

            if (builtins.contains(function_call->name()))
            {
                if (auto return_value = builtins[function_call->name()](evaluated_arguments))
                {
                    return return_value.value();
                }
                else {
                    return {};
                }
            }
            else {
                return call_function(function_call->name(), evaluated_arguments);
            }
        }
        else if (const auto number = std::dynamic_pointer_cast<ast::Number>(expression))
        {
            return number;
        }
        else if (const auto string = std::dynamic_pointer_cast<ast::String>(expression)) {

            return string;
        }
        else if (const auto symbol = std::dynamic_pointer_cast<ast::Symbol>(expression)) {

            return m_storage.lookup(symbol->name());
        }
        else if (const auto binary = std::dynamic_pointer_cast<ast::Binary>(expression)) {

            return eval_binary(binary);
        }
        else if (const auto block = std::dynamic_pointer_cast<ast::Block>(expression)) {

            m_storage.scope_begin();

            for (const auto& statement : block->statements())
                eval_expression(statement);

            m_storage.scope_end();

            return expression;
        }
        else if (const auto while_stmt = std::dynamic_pointer_cast<ast::While>(expression)) {

            eval_while(std::move(while_stmt));

            return expression;
        }
        else if (const auto if_stmt = std::dynamic_pointer_cast<ast::If>(expression)) {

            eval_if(std::move(if_stmt));

            return expression;
        }
    }

    std::vector<std::shared_ptr<ast::Object>> m_expressions;
    Storage m_storage;
};

#endif // EVAL_HPP
