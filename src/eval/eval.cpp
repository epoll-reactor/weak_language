#include "../../include/eval/builtins.hpp"
#include "../../include/eval/eval.hpp"


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

std::shared_ptr<ast::Object> Evaluator::eval_function_call(const std::shared_ptr<ast::FunctionCall>& function_call)
{
    std::vector<std::shared_ptr<ast::Object>> evaluated_arguments;

    for (const auto& arg : function_call->arguments())
        evaluated_arguments.emplace_back(eval_expression(arg));

    if (builtins.contains(function_call->name()))
    {
        if (auto function_result = builtins.at(function_call->name())(evaluated_arguments))
        {
            return function_result.value();
        }
        else {
            return {};
        }
    }

    return call_function(function_call->name(), evaluated_arguments);
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
            /// without floats
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

namespace {

class ExpressionResolver
{
public:
    explicit ExpressionResolver(std::shared_ptr<ast::Object> expr)
        : m_expr(std::move(expr)), m_result()
    { }

    template<typename AstType, typename Lambda>
    void on_type(Lambda&& lambda)
    {
        if (auto target_type = std::dynamic_pointer_cast<AstType>(m_expr))
            m_result = lambda(target_type);
    }

    std::shared_ptr<ast::Object> result() const noexcept
    {
        return m_result;
    }

private:
    std::shared_ptr<ast::Object> m_expr;
    std::shared_ptr<ast::Object> m_result;
};
} // anonymous namespace

std::shared_ptr<ast::Object> Evaluator::eval_expression(const std::shared_ptr<ast::Object>& expression)
{
    /// Doubtfully
    ExpressionResolver resolver(expression);

    resolver.on_type<ast::Number>([](const std::shared_ptr<ast::Number>& number)
    {
        return number;
    });

    resolver.on_type<ast::String>([](const std::shared_ptr<ast::String>& string)
    {
        return string;
    });

    resolver.on_type<ast::Symbol>([this](const std::shared_ptr<ast::Symbol>& symbol)
    {
        return m_storage.lookup(symbol->name());
    });

    resolver.on_type<ast::FunctionCall>([this](const std::shared_ptr<ast::FunctionCall>& function_call)
    {
        return eval_function_call(function_call);
    });

    resolver.on_type<ast::Binary>([this](const std::shared_ptr<ast::Binary>& binary)
    {
        return eval_binary(binary);
    });

    resolver.on_type<ast::Block>([this, &expression](const std::shared_ptr<ast::Block>& block)
    {
        m_storage.scope_begin();

        for (const auto& statement : block->statements())
            eval_expression(statement);

        m_storage.scope_end();

        return expression;
    });

    resolver.on_type<ast::While>([this, &expression](const std::shared_ptr<ast::While>& while_stmt)
    {
        eval_while(while_stmt);

        return expression;
    });

    resolver.on_type<ast::For>([this, &expression](const std::shared_ptr<ast::For>& for_stmt)
    {
        eval_for(for_stmt);

        return expression;
    });

    resolver.on_type<ast::If>([this, &expression](const std::shared_ptr<ast::If>& if_stmt)
    {
        eval_if(if_stmt);

        return expression;
    });

    resolver.on_type<ast::Array>([](const std::shared_ptr<ast::Array>& array)
    {
        return array;
    });

    resolver.on_type<ast::ArraySubscriptOperator>([this](const std::shared_ptr<ast::ArraySubscriptOperator>& array_subscript)
    {
        return eval_array_subscript(array_subscript);
    });

    return resolver.result();
}