#include <variant>
#include <array>

#include "../../include/eval/builtins.hpp"
#include "../../include/eval/eval.hpp"

static bool is_datatype(const std::shared_ptr<ast::Object>& object) noexcept
{
    return std::dynamic_pointer_cast<ast::Integer>(object)
        || std::dynamic_pointer_cast<ast::Float>(object)
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
        if (function) { m_storage.push(function->name(), std::move(function)); continue; }

        auto type_def = std::dynamic_pointer_cast<ast::TypeDefinition>(expr);
        if (type_def) { m_storage.push(type_def->name(), std::move(type_def)); continue; }

        throw EvalError("Only functions as global objects supported");
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
        m_storage.push(std::static_pointer_cast<ast::Symbol>(stored_function->arguments()[i])->name(), evaluated_args[i]);

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
    std::vector<std::shared_ptr<ast::Object>> arguments = function_call->arguments();

    for (auto& arg : arguments)
        arg = eval_expression(arg);

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

void Evaluator::eval_block(const std::shared_ptr<ast::Block>& block)
{
    m_storage.scope_begin();

    for (const auto& statement : block->statements())
        eval_expression(statement);

    m_storage.scope_end();
}

template <typename LeftOperand, typename RightOperand>
[[gnu::always_inline]] static constexpr bool comparison_impl(lexeme_t type, LeftOperand l, RightOperand r)
{
    switch (type)
    {
        case lexeme_t::eq:      return l == r;
        case lexeme_t::neq:     return l != r;
        case lexeme_t::ge:      return l >= r;
        case lexeme_t::gt:      return l > r;
        case lexeme_t::le:      return l <= r;
        case lexeme_t::lt:      return l < r;
        default:
            throw EvalError("Incorrect binary expression");
    }
}

template <typename LeftFloatingPoint, typename RightFloatingPoint>
[[gnu::always_inline]] static constexpr double floating_point_arithmetic_impl(lexeme_t type, LeftFloatingPoint l, RightFloatingPoint r)
{
    switch (type)
    {
        case lexeme_t::plus:    return l + r;
        case lexeme_t::minus:   return l - r;
        case lexeme_t::star:    return l * r;
        case lexeme_t::slash:   return l / r;
        default:
            return comparison_impl<LeftFloatingPoint, RightFloatingPoint>(type, l, r);
    }
}

template <typename LeftIntegral, typename RightIntegral>
[[gnu::always_inline]] static constexpr int32_t integral_arithmetic_impl(lexeme_t type, LeftIntegral l, RightIntegral r)
{
    switch (type)
    {
        case lexeme_t::plus:    return l + r;
        case lexeme_t::minus:   return l - r;
        case lexeme_t::star:    return l * r;
        case lexeme_t::slash:   return l / r;
        case lexeme_t::mod:     return l % r;
        case lexeme_t::slli:    return l << r;
        case lexeme_t::srli:    return l >> r;
        default:
            return comparison_impl<LeftIntegral, RightIntegral>(type, l, r);
    }
}

template <typename LeftOperand, typename RightOperand>
[[gnu::always_inline]] static constexpr std::variant<int32_t, double> arithmetic(lexeme_t type, const std::shared_ptr<ast::Object>& lhs, const std::shared_ptr<ast::Object>& rhs)
{
    if constexpr (std::is_same_v<ast::Integer, LeftOperand> && std::is_same_v<ast::Integer, RightOperand>)
        return integral_arithmetic_impl(
            type,
            std::static_pointer_cast<ast::Integer>(lhs)->value(),
            std::static_pointer_cast<ast::Integer>(rhs)->value()
        );

    return floating_point_arithmetic_impl(
        type,
        std::static_pointer_cast<LeftOperand>(lhs)->value(),
        std::static_pointer_cast<RightOperand>(rhs)->value()
    );
}

[[gnu::always_inline]] constexpr int32_t i_i_arithmetic(lexeme_t type, const std::shared_ptr<ast::Object>& lhs, const std::shared_ptr<ast::Object>& rhs) {
    return std::get<int32_t>(arithmetic<ast::Integer, ast::Integer>(type, lhs, rhs));
}
[[gnu::always_inline]] constexpr double i_f_arithmetic(lexeme_t type, const std::shared_ptr<ast::Object>& lhs, const std::shared_ptr<ast::Object>& rhs) {
    return std::get<double>(arithmetic<ast::Integer, ast::Float>(type, lhs, rhs));
}
[[gnu::always_inline]] constexpr double f_i_arithmetic(lexeme_t type, const std::shared_ptr<ast::Object>& lhs, const std::shared_ptr<ast::Object>& rhs) {
    return std::get<double>(arithmetic<ast::Float, ast::Integer>(type, lhs, rhs));
}
[[gnu::always_inline]] constexpr double f_f_arithmetic(lexeme_t type, const std::shared_ptr<ast::Object>& lhs, const std::shared_ptr<ast::Object>& rhs) {
    return std::get<double>(arithmetic<ast::Float, ast::Float>(type, lhs, rhs));
}

template <typename LeftArg, typename RightArg>
[[gnu::always_inline]] static constexpr bool match_type(const std::shared_ptr<ast::Object>& lhs, const std::shared_ptr<ast::Object>& rhs) noexcept
{
    return std::dynamic_pointer_cast<LeftArg>(lhs) && std::dynamic_pointer_cast<RightArg>(rhs);
}

std::shared_ptr<ast::Object> Evaluator::eval_binary(const std::shared_ptr<ast::Binary>& binary)
{
    if (binary->type() == lexeme_t::assign)
    {
        auto variable = std::static_pointer_cast<ast::Symbol>(binary->lhs());
        m_storage.overwrite(variable->name(), eval_expression(binary->rhs()));

        return binary;
    }

    auto lhs = eval_expression(binary->lhs());
    auto rhs = eval_expression(binary->rhs());

    if (match_type<ast::Integer, ast::Integer>(lhs, rhs)) { return std::make_shared<ast::Integer>(i_i_arithmetic(binary->type(), lhs, rhs)); }
    if (match_type<ast::Integer, ast::Float>(lhs, rhs))   { return std::make_shared<ast::Float>  (i_f_arithmetic(binary->type(), lhs, rhs)); }
    if (match_type<ast::Float,   ast::Integer>(lhs, rhs)) { return std::make_shared<ast::Float>  (f_i_arithmetic(binary->type(), lhs, rhs)); }
    if (match_type<ast::Float,   ast::Float>(lhs, rhs))   { return std::make_shared<ast::Float>  (f_f_arithmetic(binary->type(), lhs, rhs)); }

    throw EvalError("Unknown binary expr");
}

void Evaluator::eval_array(const std::shared_ptr<ast::Array>& array)
{
    for (auto& element : array->elements())
        element = eval_expression(element);
}

std::shared_ptr<ast::Object> Evaluator::eval_array_subscript(const std::shared_ptr<ast::ArraySubscriptOperator>& argument)
{
    auto array = std::dynamic_pointer_cast<ast::Array>(m_storage.lookup(argument->symbol_name()));
    if (!array) { throw EvalError("Try to subscript non-array expression"); }

    auto index = std::dynamic_pointer_cast<ast::Integer>(eval_expression(argument->index()));
    if (!index) { throw EvalError("Index must be integral type"); }

    auto numeric_index = static_cast<std::size_t>(index->value());
    if (array->elements().size() <= numeric_index) { throw EvalError("Out of range"); }

    return array->elements().at(numeric_index);
}

void Evaluator::eval_for(const std::shared_ptr<ast::For>& for_stmt)
{
    m_storage.scope_begin();

    auto init = std::static_pointer_cast<ast::Binary>(eval_expression(for_stmt->loop_init()));

    auto boolean_exit_condition = std::dynamic_pointer_cast<ast::Integer>(eval_expression(for_stmt->exit_condition()));
    if (!boolean_exit_condition) { throw EvalError("For loop requires bool-convertible exit condition"); }

    while (static_cast<bool>(boolean_exit_condition->value()))
    {
        eval_expression(for_stmt->body());

        eval_expression(for_stmt->increment());

        boolean_exit_condition = std::static_pointer_cast<ast::Integer>(eval_expression(for_stmt->exit_condition()));
    }

    m_storage.scope_end();
}

void Evaluator::eval_while(const std::shared_ptr<ast::While>& while_stmt)
{
    auto exit_condition = eval_expression(while_stmt->exit_condition());

    auto integral_exit_condition = std::dynamic_pointer_cast<ast::Integer>(exit_condition);
    auto floating_point_exit_condition = std::dynamic_pointer_cast<ast::Float>(exit_condition);

    if (!integral_exit_condition && !floating_point_exit_condition) { throw EvalError("While requires bool-convertible exit condition"); }

    if (integral_exit_condition)
    {
        while (static_cast<bool>(integral_exit_condition->value()))
        {
            eval_expression(while_stmt->body());
            integral_exit_condition = std::static_pointer_cast<ast::Integer>(eval_expression(while_stmt->exit_condition()));
        }
    }

    if (floating_point_exit_condition)
    {
        while (static_cast<bool>(floating_point_exit_condition->value()))
        {
            eval_expression(while_stmt->body());
            floating_point_exit_condition = std::static_pointer_cast<ast::Float>(eval_expression(while_stmt->exit_condition()));
        }
    }
}

void Evaluator::eval_if(const std::shared_ptr<ast::If>& if_stmt)
{
    auto if_condition = eval_expression(if_stmt->condition());

    if (static_cast<bool>(std::static_pointer_cast<ast::Integer>(if_condition)->value()))
    {
        eval_expression(if_stmt->body());
    }
    else if (auto else_body = if_stmt->else_body()) {

        eval_expression(else_body);
    }
}

std::shared_ptr<ast::Object> Evaluator::eval_expression(const std::shared_ptr<ast::Object>& expression)
{
    if (auto target = std::dynamic_pointer_cast<ast::Integer>(expression))      { return target; }
    if (auto target = std::dynamic_pointer_cast<ast::Float>(expression))        { return target; }
    if (auto target = std::dynamic_pointer_cast<ast::String>(expression))       { return target; }
    if (auto target = std::dynamic_pointer_cast<ast::Symbol>(expression))       { return m_storage.lookup(target->name()); }
    if (auto target = std::dynamic_pointer_cast<ast::Binary>(expression))       { return eval_binary(target); }
    if (auto target = std::dynamic_pointer_cast<ast::FunctionCall>(expression)) { return eval_function_call(target); }
    if (auto target = std::dynamic_pointer_cast<ast::ArraySubscriptOperator>(expression)) { return eval_array_subscript(target); }
    if (auto target = std::dynamic_pointer_cast<ast::Array>(expression))        { eval_array(target); return expression; }
    if (auto target = std::dynamic_pointer_cast<ast::Block>(expression))        { eval_block(target); return expression; }
    if (auto target = std::dynamic_pointer_cast<ast::While>(expression))        { eval_while(target); return expression; }
    if (auto target = std::dynamic_pointer_cast<ast::For>(expression))          { eval_for(target); return expression; }
    if (auto target = std::dynamic_pointer_cast<ast::If>(expression))           { eval_if(target); return expression; }
    throw EvalError("Unknown expression");
}
