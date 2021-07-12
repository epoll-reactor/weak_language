#include "../../include/eval/eval.hpp"

#include "../../include/eval/implementation/binary.hpp"
#include "../../include/eval/implementation/unary.hpp"

#include "../../include/common_defs.hpp"
#include "../../include/error/eval_error.hpp"
#include "../../include/std/builtins.hpp"

ALWAYS_INLINE static bool is_datatype(const ast::Object* object) noexcept
{
    if (!object) { return false; }

    return object->ast_type() == ast::ast_type_t::INTEGER
        || object->ast_type() == ast::ast_type_t::FLOAT
        || object->ast_type() == ast::ast_type_t::STRING
        || object->ast_type() == ast::ast_type_t::ARRAY;
}

Evaluator::Evaluator(const boost::local_shared_ptr<ast::RootObject>& program)
{
    for (const auto& stmt : program->get())
    {
        m_expressions.emplace_back(stmt);
    }
}

void Evaluator::eval()
{
    for (const auto& expr : m_expressions)
    {
        auto function = boost::dynamic_pointer_cast<ast::Function>(expr);
        if (function) { m_storage.push(function->name().data(), function); continue; }

        auto type_def = boost::dynamic_pointer_cast<ast::TypeDefinition>(expr);
        if (type_def) { m_storage.push(type_def->name().data(), type_def); continue; }

        throw EvalError("Only functions as global objects supported");
    }

    call_function("main", {});
}

boost::local_shared_ptr<ast::Object> Evaluator::call_function(std::string_view name, const std::vector<boost::local_shared_ptr<ast::Object>>& evaluated_args)
{
    auto stored_function = m_storage.lookup(name.data());
    if (stored_function->ast_type() != ast::ast_type_t::FUNCTION) { throw EvalError("Try to call not a function"); }

    auto function = static_cast<ast::Function*>(stored_function.get());

    if (UNLIKELY(function->arguments().size() != evaluated_args.size()))
        throw EvalError("Wrong arguments size");

    boost::local_shared_ptr<ast::Object> last_statement;

    m_storage.scope_begin();

    for (std::size_t i = 0; i < evaluated_args.size(); ++i)
        m_storage.push(static_cast<ast::Symbol*>(function->arguments()[i].get())->name().data(), evaluated_args[i]);

    for (const auto& arg : function->body()->statements())
        last_statement = eval_expression(arg);

    m_storage.scope_end();

    if (is_datatype(last_statement.get()))
        return last_statement;
    else
        return {};
}

boost::local_shared_ptr<ast::Object> Evaluator::eval_function_call(const boost::local_shared_ptr<ast::FunctionCall>& function_call)
{
    auto arguments = function_call->arguments();

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

void Evaluator::eval_block(const boost::local_shared_ptr<ast::Block>& block)
{
    m_storage.scope_begin();

    for (const auto& statement : block->statements())
        eval_expression(statement);

    m_storage.scope_end();
}

boost::local_shared_ptr<ast::Object> Evaluator::eval_binary(const boost::local_shared_ptr<ast::Binary>& binary)
{
    if (binary->type() == lexeme_t::assign)
    {
        auto variable = boost::static_pointer_cast<ast::Symbol>(binary->lhs());
        m_storage.overwrite(variable->name(), eval_expression(binary->rhs()));

        return binary;
    }

    auto lhs = eval_expression(binary->lhs());
    auto rhs = eval_expression(binary->rhs());

    ast::ast_type_t lhs_binary_type = lhs->ast_type();
    ast::ast_type_t rhs_binary_type = rhs->ast_type();

    if (lhs_binary_type == ast::ast_type_t::INTEGER && rhs_binary_type == ast::ast_type_t::INTEGER) { return internal::i_i_binary_implementation(binary->type(), lhs, rhs); }
    if (lhs_binary_type == ast::ast_type_t::INTEGER && rhs_binary_type == ast::ast_type_t::FLOAT)   { return internal::i_f_binary_implementation(binary->type(), lhs, rhs); }
    if (lhs_binary_type == ast::ast_type_t::FLOAT   && rhs_binary_type == ast::ast_type_t::INTEGER) { return internal::f_i_binary_implementation(binary->type(), lhs, rhs); }
    if (lhs_binary_type == ast::ast_type_t::FLOAT   && rhs_binary_type == ast::ast_type_t::FLOAT)   { return internal::f_f_binary_implementation(binary->type(), lhs, rhs); }

    throw EvalError("Unknown binary expr");
}

boost::local_shared_ptr<ast::Object> Evaluator::eval_unary(const boost::local_shared_ptr<ast::Unary>& unary)
{
    boost::local_shared_ptr<ast::Object> operand = unary->operand();
    lexeme_t type = unary->type();
    ast::ast_type_t ast_type = operand->ast_type();

    auto unary_result = internal::unary_implementation(ast_type, type, operand);
    if (unary_result) { return unary_result; }

    if (ast_type != ast::ast_type_t::SYMBOL) { throw EvalError("Unknown unary operand type"); }
    auto variable = boost::static_pointer_cast<ast::Symbol>(operand);
    auto symbol = m_storage.lookup(variable->name());
    m_storage.overwrite(variable->name(), internal::unary_implementation(symbol->ast_type(), type, symbol));

    return symbol;
}

void Evaluator::eval_array(const boost::local_shared_ptr<ast::Array>& array)
{
    for (auto& element : array->elements())
        element = eval_expression(element);
}

boost::local_shared_ptr<ast::Object> Evaluator::eval_array_subscript(const boost::local_shared_ptr<ast::ArraySubscriptOperator>& argument)
{
    auto array_object = m_storage.lookup(argument->symbol_name());
    if (array_object->ast_type() != ast::ast_type_t::ARRAY) { throw EvalError("Try to subscript non-array expression"); }
    auto array = boost::static_pointer_cast<ast::Array>(array_object);

    auto index = eval_expression(argument->index());
    if (index->ast_type() != ast::ast_type_t::INTEGER) { throw EvalError("Index must be integral type"); }

    auto casted_index = boost::static_pointer_cast<ast::Integer>(index);
    auto casted_array = boost::static_pointer_cast<ast::Array>(array);

    auto numeric_index = static_cast<std::size_t>(casted_index->value());
    if (casted_array->elements().size() <= numeric_index) { throw EvalError("Out of range"); }

    return casted_array->elements().at(numeric_index);
}

void Evaluator::eval_for(const boost::local_shared_ptr<ast::For>& for_stmt)
{
    m_storage.scope_begin();

    auto init = boost::static_pointer_cast<ast::Binary>(eval_expression(for_stmt->loop_init()));

    auto exit_cond = for_stmt->exit_condition();
    auto increment = for_stmt->increment();
    auto body = for_stmt->body();
    auto boolean_exit_condition = boost::static_pointer_cast<ast::Integer>(eval_expression(exit_cond));

    while (LIKELY(boolean_exit_condition->value()))
    {
        eval_expression(body);

        eval_expression(increment);

        boolean_exit_condition = boost::static_pointer_cast<ast::Integer>(eval_expression(exit_cond));
    }

    m_storage.scope_end();
}

void Evaluator::eval_while(const boost::local_shared_ptr<ast::While>& while_stmt)
{
    auto initial_exit_condition = eval_expression(while_stmt->exit_condition());
    ast::ast_type_t exit_condition_type = initial_exit_condition->ast_type();

    auto while_implementation = [this, &while_stmt, initial_exit_condition = std::move(initial_exit_condition)]<typename IntegralType>{
        auto body = while_stmt->body();
        auto exit_cond = boost::static_pointer_cast<IntegralType>(initial_exit_condition);

        while (LIKELY(exit_cond->value()))
        {
            eval_expression(body);
            exit_cond = boost::static_pointer_cast<IntegralType>(eval_expression(while_stmt->exit_condition()));
        }
    };

    if (exit_condition_type == ast::ast_type_t::INTEGER)
    {
        while_implementation.template operator()<ast::Integer>();
        return;
    }

    if (exit_condition_type == ast::ast_type_t::FLOAT)
    {
        while_implementation.template operator()<ast::Integer>();
        return;
    }
}

void Evaluator::eval_if(const boost::local_shared_ptr<ast::If>& if_stmt)
{
    auto if_condition = eval_expression(if_stmt->condition());

    if (boost::static_pointer_cast<ast::Integer>(if_condition)->value())
    {
        eval_expression(if_stmt->body());
    }
    else if (auto else_body = if_stmt->else_body()) {

        eval_expression(else_body);
    }
}

boost::local_shared_ptr<ast::Object> Evaluator::eval_expression(const boost::local_shared_ptr<ast::Object>& expression)
{
    ast::ast_type_t expr_type = expression->ast_type();

    switch (expr_type)
    {
        case ast::ast_type_t::INTEGER:
        case ast::ast_type_t::FLOAT:
        case ast::ast_type_t::STRING:
            return expression;
        case ast::ast_type_t::SYMBOL:           return m_storage.lookup(boost::static_pointer_cast<ast::Symbol>(expression)->name());
        case ast::ast_type_t::ARRAY:            eval_array(boost::static_pointer_cast<ast::Array>(expression)); return expression;
        case ast::ast_type_t::FUNCTION_CALL:    return eval_function_call(boost::static_pointer_cast<ast::FunctionCall>(expression));
        case ast::ast_type_t::BINARY:           return eval_binary(boost::static_pointer_cast<ast::Binary>(expression));
        case ast::ast_type_t::UNARY:            return eval_unary(boost::static_pointer_cast<ast::Unary>(expression));
        case ast::ast_type_t::ARRAY_SUBSCRIPT_OPERATOR: return eval_array_subscript(boost::static_pointer_cast<ast::ArraySubscriptOperator>(expression));
        case ast::ast_type_t::BLOCK:            eval_block(boost::static_pointer_cast<ast::Block>(expression)); return expression;
        case ast::ast_type_t::WHILE:            eval_while(boost::static_pointer_cast<ast::While>(expression)); return expression;
        case ast::ast_type_t::FOR:              eval_for(boost::static_pointer_cast<ast::For>(expression)); return expression;
        case ast::ast_type_t::IF:               eval_if(boost::static_pointer_cast<ast::If>(expression)); return expression;
        default:
            throw EvalError("Unknown expression");
    }
}
