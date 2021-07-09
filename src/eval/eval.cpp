#include <variant>
#include <array>

#include "../../include/eval/builtins.hpp"
#include "../../include/eval/eval.hpp"


static bool is_datatype(const ast::Object* object) noexcept
{
    if (!object) { return false; }

    return object->ast_type() == ast::ast_type_t::INTEGER
        || object->ast_type() == ast::ast_type_t::FLOAT
        || object->ast_type() == ast::ast_type_t::STRING
        || object->ast_type() == ast::ast_type_t::ARRAY;
}

Evaluator::Evaluator(const std::shared_ptr<ast::RootObject>& program)
    : m_expressions(program->get())
{ }

void Evaluator::eval()
{
    for (const auto& expr : m_expressions)
    {
        auto function = std::dynamic_pointer_cast<ast::Function>(expr);
        if (function) { m_storage.push(function->name().data(), function); continue; }

        auto type_def = std::dynamic_pointer_cast<ast::TypeDefinition>(expr);
        if (type_def) { m_storage.push(type_def->name().data(), type_def); continue; }

        throw EvalError("Only functions as global objects supported");
    }

    call_function("main", {});
}

std::shared_ptr<ast::Object> Evaluator::call_function(std::string_view name, const std::vector<std::shared_ptr<ast::Object>>& evaluated_args)
{
    auto stored_function = std::dynamic_pointer_cast<ast::Function>(m_storage.lookup(name.data()));
    if (!stored_function) { throw EvalError("Try to call not a function"); }

    if (stored_function->arguments().size() != evaluated_args.size())
        throw EvalError("Wrong arguments size");

    std::shared_ptr<ast::Object> last_statement;

    m_storage.scope_begin();

    /// Load function arguments to local scope
    for (std::size_t i = 0; i < evaluated_args.size(); ++i)
        m_storage.push(std::static_pointer_cast<ast::Symbol>(stored_function->arguments()[i])->name().data(), evaluated_args[i]);

    for (const auto& arg : stored_function->body()->statements())
        last_statement = eval_expression(arg);

    m_storage.scope_end();

    if (is_datatype(last_statement.get()))
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
[[gnu::always_inline]] static bool comparison_impl(lexeme_t type, LeftOperand l, RightOperand r)
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
[[gnu::always_inline]] static double floating_point_arithmetic_impl(lexeme_t type, LeftFloatingPoint l, RightFloatingPoint r)
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
[[gnu::always_inline]] static constexpr std::variant<int32_t, double> arithmetic(lexeme_t type, const ast::Object* lhs, const ast::Object* rhs)
{
    if constexpr (std::is_same_v<ast::Integer, LeftOperand> && std::is_same_v<ast::Integer, RightOperand>)
        return integral_arithmetic_impl(
            type,
            static_cast<const ast::Integer*>(lhs)->value(),
            static_cast<const ast::Integer*>(rhs)->value()
        );

    return floating_point_arithmetic_impl(
        type,
        static_cast<const LeftOperand*>(lhs)->value(),
        static_cast<const RightOperand*>(rhs)->value()
    );
}

[[gnu::always_inline]] constexpr int32_t i_i_arithmetic(lexeme_t type, const ast::Object* lhs, const ast::Object* rhs) {
    return std::get<int32_t>(arithmetic<ast::Integer, ast::Integer>(type, lhs, rhs));
}
[[gnu::always_inline]] constexpr double i_f_arithmetic(lexeme_t type, const ast::Object* lhs, const ast::Object* rhs) {
    return std::get<double>(arithmetic<ast::Integer, ast::Float>(type, lhs, rhs));
}
[[gnu::always_inline]] constexpr double f_i_arithmetic(lexeme_t type, const ast::Object* lhs, const ast::Object* rhs) {
    return std::get<double>(arithmetic<ast::Float, ast::Integer>(type, lhs, rhs));
}
[[gnu::always_inline]] constexpr double f_f_arithmetic(lexeme_t type, const ast::Object* lhs, const ast::Object* rhs) {
    return std::get<double>(arithmetic<ast::Float, ast::Float>(type, lhs, rhs));
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

    ast::ast_type_t lhs_binary_type = lhs->ast_type();
    ast::ast_type_t rhs_binary_type = rhs->ast_type();

    if (lhs_binary_type == ast::ast_type_t::INTEGER && rhs_binary_type == ast::ast_type_t::INTEGER) { return pool_allocate<ast::Integer>(i_i_arithmetic(binary->type(), lhs.get(), rhs.get())); }
    if (lhs_binary_type == ast::ast_type_t::INTEGER && rhs_binary_type == ast::ast_type_t::FLOAT)   { return pool_allocate<ast::Float>  (i_f_arithmetic(binary->type(), lhs.get(), rhs.get())); }
    if (lhs_binary_type == ast::ast_type_t::FLOAT   && rhs_binary_type == ast::ast_type_t::INTEGER) { return pool_allocate<ast::Float>  (f_i_arithmetic(binary->type(), lhs.get(), rhs.get())); }
    if (lhs_binary_type == ast::ast_type_t::FLOAT   && rhs_binary_type == ast::ast_type_t::FLOAT)   { return pool_allocate<ast::Float>  (f_f_arithmetic(binary->type(), lhs.get(), rhs.get())); }

    throw EvalError("Unknown binary expr");
}

void Evaluator::eval_array(const std::shared_ptr<ast::Array>& array)
{
    for (auto& element : array->elements())
        element = eval_expression(element);
}

std::shared_ptr<ast::Object> Evaluator::eval_array_subscript(const std::shared_ptr<ast::ArraySubscriptOperator>& argument)
{
    auto array_object = m_storage.lookup(argument->symbol_name());
    if (array_object->ast_type() != ast::ast_type_t::ARRAY) { throw EvalError("Try to subscript non-array expression"); }
    auto array = std::static_pointer_cast<ast::Array>(array_object);

    auto index = eval_expression(argument->index());
    if (index->ast_type() != ast::ast_type_t::INTEGER) { throw EvalError("Index must be integral type"); }

    auto casted_index = std::static_pointer_cast<ast::Integer>(index);
    auto casted_array = std::static_pointer_cast<ast::Array>(array);

    auto numeric_index = static_cast<std::size_t>(casted_index->value());
    if (casted_array->elements().size() <= numeric_index) { throw EvalError("Out of range"); }

    return casted_array->elements().at(numeric_index);
}

void Evaluator::eval_for(const std::shared_ptr<ast::For>& for_stmt)
{
    m_storage.scope_begin();

    auto init = std::static_pointer_cast<ast::Binary>(eval_expression(for_stmt->loop_init()));

    auto boolean_exit_condition = std::static_pointer_cast<ast::Integer>(eval_expression(for_stmt->exit_condition()));

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

    ast::ast_type_t exit_condition_type = exit_condition->ast_type();

    if (exit_condition_type == ast::ast_type_t::INTEGER)
    {
        std::shared_ptr<ast::Integer> integral_exit_cond = std::static_pointer_cast<ast::Integer>(exit_condition);
        while (static_cast<bool>(integral_exit_cond->value()))
        {
            eval_expression(while_stmt->body());
            integral_exit_cond = std::static_pointer_cast<ast::Integer>(eval_expression(while_stmt->exit_condition()));
        }
        return;
    }

    if (exit_condition_type == ast::ast_type_t::FLOAT)
    {
        std::shared_ptr<ast::Float> float_exit_cond = std::static_pointer_cast<ast::Float>(exit_condition);
        while (static_cast<bool>(float_exit_cond->value()))
        {
            eval_expression(while_stmt->body());
            float_exit_cond = std::static_pointer_cast<ast::Float>(eval_expression(while_stmt->exit_condition()));
        }
        return;
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
    ast::ast_type_t expr_type = expression->ast_type();

    if (expr_type == ast::ast_type_t::INTEGER)      { return expression; }
    if (expr_type == ast::ast_type_t::FLOAT)        { return expression; }
    if (expr_type == ast::ast_type_t::STRING)       { return expression; }
    if (expr_type == ast::ast_type_t::SYMBOL)       { return m_storage.lookup(std::static_pointer_cast<ast::Symbol>(expression)->name()); }
    if (expr_type == ast::ast_type_t::BINARY)       { return eval_binary(std::static_pointer_cast<ast::Binary>(expression)); }
    if (expr_type == ast::ast_type_t::FUNCTION_CALL) { return eval_function_call(std::static_pointer_cast<ast::FunctionCall>(expression)); }
    if (expr_type == ast::ast_type_t::ARRAY_SUBSCRIPT_OPERATOR) { return eval_array_subscript(std::static_pointer_cast<ast::ArraySubscriptOperator>(expression)); }
    if (expr_type == ast::ast_type_t::ARRAY)        { eval_array(std::static_pointer_cast<ast::Array>(expression)); return expression; }
    if (expr_type == ast::ast_type_t::BLOCK)        { eval_block(std::static_pointer_cast<ast::Block>(expression)); return expression; }
    if (expr_type == ast::ast_type_t::WHILE)        { eval_while(std::static_pointer_cast<ast::While>(expression)); return expression; }
    if (expr_type == ast::ast_type_t::FOR)          { eval_for(std::static_pointer_cast<ast::For>(expression)); return expression; }
    if (expr_type == ast::ast_type_t::IF)           { eval_if(std::static_pointer_cast<ast::If>(expression)); return expression; }

    throw EvalError("Unknown expression");
}
