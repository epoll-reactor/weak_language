#include <variant>
#include <map>

#include "../../include/eval/builtins.hpp"
#include "../../include/eval/eval.hpp"

static bool is_datatype(const std::shared_ptr<ast::Object>& object) noexcept
{
    return std::dynamic_pointer_cast<ast::Integer>(object)
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
        if (function) { m_storage.push(function->name(), function); continue; }

        auto type_def = std::dynamic_pointer_cast<ast::TypeDefinition>(expr);
        if (type_def) { m_storage.push(type_def->name(), type_def); continue; }

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

void Evaluator::eval_block(const std::shared_ptr<ast::Block>& block)
{
    m_storage.scope_begin();

    for (const auto& statement : block->statements())
        eval_expression(statement);

    m_storage.scope_end();
}

template <typename LeftOperand, typename RightOperand>
static constexpr bool comparison_impl(lexeme_t type, LeftOperand l, RightOperand r)
{
    if constexpr (!std::is_same_v<LeftOperand, RightOperand>)
        throw EvalError("Comparisons of different numeric types are forbidden");

    switch (type)
    {
        case lexeme_t::eq:      return l == r;
        case lexeme_t::neq:     return l != r;
        case lexeme_t::ge:      return l >= r;
        case lexeme_t::gt:      return l > r;
        case lexeme_t::le:      return l <= r;
        case lexeme_t::lt:      return l < r;
        default:
            throw EvalError("Unsupported binary operator");
    }
}

template <typename LeftIntegral, typename RightIntegral>
static constexpr double floating_point_arithmetic_impl(lexeme_t type, LeftIntegral l, RightIntegral r)
{
    switch (type)
    {
        case lexeme_t::plus:    return l + r;
        case lexeme_t::minus:   return l - r;
        case lexeme_t::star:    return l * r;
        case lexeme_t::slash:   return l / r;
        default:
            return comparison_impl<LeftIntegral, RightIntegral>(type, l, r);
    }
}

template <typename LeftIntegral, typename RightIntegral>
static constexpr int32_t integral_arithmetic_impl(lexeme_t type, LeftIntegral l, RightIntegral r)
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
static constexpr std::variant<int32_t, double> arithmetic(lexeme_t type, const std::shared_ptr<ast::Object>& lhs, const std::shared_ptr<ast::Object>& rhs)
{
    if constexpr (std::is_same_v<ast::Integer, LeftOperand> && std::is_same_v<ast::Integer, RightOperand>)
        return integral_arithmetic_impl(
            type,
            std::dynamic_pointer_cast<ast::Integer>(lhs)->value(),
            std::dynamic_pointer_cast<ast::Integer>(rhs)->value()
        );

    return floating_point_arithmetic_impl(
        type,
        std::dynamic_pointer_cast<LeftOperand>(lhs)->value(),
        std::dynamic_pointer_cast<RightOperand>(rhs)->value()
    );
}

template <typename LeftIntegral, typename RightIntegral>
static constexpr std::variant<int32_t, double> binary(lexeme_t type, const std::shared_ptr<ast::Object>& lhs, const std::shared_ptr<ast::Object>& rhs)
{
    static_assert(std::is_same_v<int32_t, LeftIntegral>  || std::is_same_v<double, LeftIntegral>);
    static_assert(std::is_same_v<int32_t, RightIntegral> || std::is_same_v<double, RightIntegral>);

    auto relation = []<typename LeftArg, typename RightArg>() noexcept {
        return std::is_same_v<LeftArg, LeftIntegral> && std::is_same_v<RightArg, RightIntegral>;
    };

    if constexpr (relation.template operator()<int32_t, int32_t>())     { return arithmetic<ast::Integer, ast::Integer>(type, lhs, rhs); }
    else if constexpr (relation.template operator()<double, double>())  { return arithmetic<ast::Float, ast::Float>(type, lhs, rhs); }
    else if constexpr (relation.template operator()<double, int32_t>()) { return arithmetic<ast::Float, ast::Integer>(type, lhs, rhs); }
    else if constexpr (relation.template operator()<int32_t, double>()) { return arithmetic<ast::Integer, ast::Float>(type, lhs, rhs); }
};

template <typename LeftArg, typename RightArg>
bool relation(auto lhs, auto rhs)
{
    return std::dynamic_pointer_cast<LeftArg>(lhs) && std::dynamic_pointer_cast<RightArg>(rhs);
}

std::shared_ptr<ast::Object> Evaluator::eval_binary(const std::shared_ptr<ast::Binary>& binary)
{
    if (binary->type() == lexeme_t::assign)
    {
        auto variable = std::dynamic_pointer_cast<ast::Symbol>(binary->lhs());
        m_storage.overwrite(variable->name(), eval_expression(binary->rhs()));

        return binary;
    }

    auto lhs = eval_expression(binary->lhs());
    auto rhs = eval_expression(binary->rhs());

    enum struct integer_float_rel { i_i, i_f, f_i, f_f };

    std::unordered_map<integer_float_rel, std::function<std::variant<int32_t, double>()>> type_relations = z{
        {integer_float_rel::i_i, [&lhs, &rhs, &binary] { return ::binary<int32_t, int32_t>(binary->type(), lhs, rhs); }},
        {integer_float_rel::i_f, [&lhs, &rhs, &binary] { return ::binary<int32_t, double> (binary->type(), lhs, rhs); }},
        {integer_float_rel::f_i, [&lhs, &rhs, &binary] { return ::binary<double,  int32_t>(binary->type(), lhs, rhs); }},
        {integer_float_rel::f_f, [&lhs, &rhs, &binary] { return ::binary<double,  double> (binary->type(), lhs, rhs); }}
    };

    if (relation<ast::Integer, ast::Integer>(lhs, rhs)) { return std::make_shared<ast::Integer>(std::get<int32_t>(type_relations[integer_float_rel::i_i]())); }
    if (relation<ast::Integer, ast::Float>(lhs, rhs))   { return std::make_shared<ast::Float>  (std::get<double> (type_relations[integer_float_rel::i_f]())); }
    if (relation<ast::Float, ast::Integer>(lhs, rhs))   { return std::make_shared<ast::Float>  (std::get<double> (type_relations[integer_float_rel::f_i]())); }
    if (relation<ast::Float, ast::Float>(lhs, rhs))     { return std::make_shared<ast::Float>  (std::get<double> (type_relations[integer_float_rel::f_f]())); }

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
    auto init = std::dynamic_pointer_cast<ast::Binary>(eval_expression(for_stmt->loop_init()));

    auto boolean_exit_condition = std::dynamic_pointer_cast<ast::Integer>(eval_expression(for_stmt->exit_condition()));
    if (!boolean_exit_condition) { throw EvalError("For loop requires bool-convertible exit condition"); }

    while (static_cast<bool>(boolean_exit_condition->value()))
    {
        eval_expression(for_stmt->body());

        eval_expression(for_stmt->increment());

        boolean_exit_condition = std::dynamic_pointer_cast<ast::Integer>(eval_expression(for_stmt->exit_condition()));
    }

    m_storage.scope_end();
}

void Evaluator::eval_while(const std::shared_ptr<ast::While>& while_stmt)
{
    auto exit_condition = eval_expression(while_stmt->exit_condition());

    auto boolean_exit_condition = std::dynamic_pointer_cast<ast::Integer>(exit_condition);
    if (!boolean_exit_condition) { throw EvalError("While requires bool-convertible exit condition"); }

    while (static_cast<bool>(boolean_exit_condition->value()))
    {
        eval_expression(while_stmt->body());
        boolean_exit_condition = std::dynamic_pointer_cast<ast::Integer>(eval_expression(while_stmt->exit_condition()));
    }
}

void Evaluator::eval_if(const std::shared_ptr<ast::If>& if_stmt)
{
    auto if_condition = eval_expression(if_stmt->condition());

    if (static_cast<bool>(std::dynamic_pointer_cast<ast::Integer>(if_condition)->value()))
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
