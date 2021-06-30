#include "../../include/eval/builtins.hpp"
#include "../../include/eval/eval.hpp"

static bool is_datatype(const std::shared_ptr<ast::Object>& object) noexcept
{
    return std::dynamic_pointer_cast<ast::Number>(object)
        || std::dynamic_pointer_cast<ast::String>(object)
        || std::dynamic_pointer_cast<ast::Array>(object);
}

Evaluator::Evaluator(const std::shared_ptr<ast::RootObject>& program)
    : m_expressions(program->get())
{ }

void Evaluator::eval()
{
    for (const auto& expr : m_expressions)
    {
        auto function = std::dynamic_pointer_cast<ast::Function>(expr);
        if (!function) { throw EvalError("Only functions as global objects supported"); }

        m_storage.push(function->name(), function);
    }

    call_function("main", {});
}

std::shared_ptr<ast::Object> Evaluator::call_function(std::string_view name, const std::vector<std::shared_ptr<ast::Object>>& evaluated_args)
{
    auto stored_function = std::dynamic_pointer_cast<ast::Function>(m_storage.lookup(name));
    if (!stored_function) { throw EvalError("Try to call not a function"); }

    if (stored_function->arguments().size() != evaluated_args.size())
        throw EvalError("Wrong arguments size");

    std::shared_ptr<ast::Object> last_statement;

    m_storage.scope_begin();

    /// Load function arguments to local scope
    for (std::size_t i = 0; i < evaluated_args.size(); ++i)
        m_storage.push(std::dynamic_pointer_cast<ast::Symbol>(stored_function->arguments()[i])->name(), evaluated_args[i]);

    for (const auto& arg : stored_function->body()->statements())
        last_statement = eval_expression(arg);

    m_storage.scope_end();

    if (is_datatype(last_statement))
        return last_statement;
    else
        return {};
}

std::shared_ptr<ast::Object> Evaluator::eval_function_call(const std::shared_ptr<ast::FunctionCall>& function_call)
{
    std::vector<std::shared_ptr<ast::Object>> arguments;

    for (const auto& arg : function_call->arguments())
        arguments.emplace_back(eval_expression(arg));

    if (builtins.contains(function_call->name()))
    {
        if (auto function_result = builtins.at(function_call->name())(arguments))
        {
            return function_result.value();
        }
        else {
            return {};
        }
    }

    return call_function(function_call->name(), arguments);
}

std::shared_ptr<ast::Object> Evaluator::eval_block(const std::shared_ptr<ast::Block>& block)
{
    m_storage.scope_begin();

    for (const auto& statement : block->statements())
        eval_expression(statement);

    m_storage.scope_end();

    return block;
}

std::shared_ptr<ast::Object> Evaluator::eval_binary(const std::shared_ptr<ast::Binary>& binary)
{
    auto binary_impl = [&](const std::shared_ptr<ast::Binary>& binary) {

        auto left_num = std::dynamic_pointer_cast<ast::Number>(eval_expression(binary->lhs()));
        auto right_num = std::dynamic_pointer_cast<ast::Number>(eval_expression(binary->rhs()));

        if (!left_num || !right_num)
            throw EvalError("Unsupported binary operands");

        double l = left_num->value();
        double r = right_num->value();
        double result = 0.0f;

        switch (binary->type())
        {
            case lexeme_t::plus:    result = l + r; break;
            case lexeme_t::minus:   result = l - r; break;
            case lexeme_t::star:    result = l * r; break;
            case lexeme_t::slash:   result = l / r; break;
            /// Consequence of ill-conceived lexical analysis
            /// without separating floats and ints
            case lexeme_t::mod:     result = static_cast<int>(l) % static_cast<int>(r); break;
            case lexeme_t::eq:      result = l == r; break;
            case lexeme_t::neq:     result = l != r; break;
            case lexeme_t::ge:      result = l >= r; break;
            case lexeme_t::gt:      result = l > r; break;
            case lexeme_t::le:      result = l <= r; break;
            case lexeme_t::lt:      result = l < r; break;
            default:
                throw EvalError("Unsupported binary operator");
        }

        return std::make_shared<ast::Number>(result);
    };

    if (binary->type() == lexeme_t::assign)
    {
        auto variable = std::dynamic_pointer_cast<ast::Symbol>(binary->lhs());

        m_storage.overwrite(variable->name(), eval_expression(binary->rhs()));

        return binary;
    }

    return binary_impl(binary);
}

std::shared_ptr<ast::Object> Evaluator::eval_array(const std::shared_ptr<ast::Array>& array)
{
    for (auto& element : array->elements())
        element = eval_expression(element);

    return array;
}

void Evaluator::eval_while(const std::shared_ptr<ast::While>& while_stmt)
{
    auto exit_condition = eval_expression(while_stmt->exit_condition());

    auto boolean_exit_condition = std::dynamic_pointer_cast<ast::Number>(exit_condition);
    if (!boolean_exit_condition) { throw EvalError("While requires bool-convertible exit condition"); }

    while (static_cast<bool>(boolean_exit_condition->value()))
    {
        eval_expression(while_stmt->body());
        boolean_exit_condition = std::dynamic_pointer_cast<ast::Number>(eval_expression(while_stmt->exit_condition()));
    }
}

std::shared_ptr<ast::Object> Evaluator::eval_array_subscript(const std::shared_ptr<ast::ArraySubscriptOperator>& argument)
{
    auto array = std::dynamic_pointer_cast<ast::Array>(m_storage.lookup(argument->symbol_name()));
    if (!array) { throw EvalError("Try to subscript non-array expression"); }

    auto index = std::dynamic_pointer_cast<ast::Number>(eval_expression(argument->index()));
    if (!index) { throw EvalError("Index must be integral type"); }

    auto numeric_index = static_cast<std::size_t>(index->value());
    if (array->elements().size() <= numeric_index) { throw EvalError("Out of range"); }

    return array->elements().at(numeric_index);
}

void Evaluator::eval_if(const std::shared_ptr<ast::If>& if_stmt)
{
    auto if_condition = eval_expression(if_stmt->condition());

    if (static_cast<bool>(std::dynamic_pointer_cast<ast::Number>(if_condition)->value()))
    {
        eval_expression(if_stmt->body());
    }
    else if (auto else_body = if_stmt->else_body()) {

        eval_expression(else_body);
    }
}

void Evaluator::eval_for(const std::shared_ptr<ast::For>& for_stmt)
{
    auto init = std::dynamic_pointer_cast<ast::Binary>(eval_expression(for_stmt->loop_init()));

    auto boolean_exit_condition = std::dynamic_pointer_cast<ast::Number>(eval_expression(for_stmt->exit_condition()));
    if (!boolean_exit_condition) { throw EvalError("For loop requires bool-convertible exit condition"); }

    while (static_cast<bool>(boolean_exit_condition->value()))
    {
        eval_expression(for_stmt->body());

        eval_expression(for_stmt->increment());

        boolean_exit_condition = std::dynamic_pointer_cast<ast::Number>(eval_expression(for_stmt->exit_condition()));
    }

    m_storage.scope_end();
}

std::shared_ptr<ast::Object> Evaluator::eval_expression(const std::shared_ptr<ast::Object>& expression)
{
    if (auto target = std::dynamic_pointer_cast<ast::Number>(expression))
        return target;

    if (auto target = std::dynamic_pointer_cast<ast::String>(expression))
        return target;

    if (auto target = std::dynamic_pointer_cast<ast::Symbol>(expression))
        return m_storage.lookup(target->name());

    if (auto target = std::dynamic_pointer_cast<ast::Array>(expression))
        return eval_array(target);

    if (auto target = std::dynamic_pointer_cast<ast::Binary>(expression))
        return eval_binary(target);

    if (auto target = std::dynamic_pointer_cast<ast::Block>(expression))
        return eval_block(target);

    if (auto target = std::dynamic_pointer_cast<ast::ArraySubscriptOperator>(expression))
        return eval_array_subscript(target);

    if (auto target = std::dynamic_pointer_cast<ast::FunctionCall>(expression))
        return eval_function_call(target);

    if (auto target = std::dynamic_pointer_cast<ast::While>(expression))
        { eval_while(target); return expression; }

    if (auto target = std::dynamic_pointer_cast<ast::For>(expression))
        { eval_for(target); return expression; }

    if (auto target = std::dynamic_pointer_cast<ast::If>(expression))
        { eval_if(target); return expression; }

    throw EvalError("Unknown expression");
}
