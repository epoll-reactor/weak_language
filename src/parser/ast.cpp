#include "../../include/parser/ast.hpp"

namespace ast {

std::vector<std::shared_ptr<Object>> RootObject::get()
{
    return m_expressions;
}

void RootObject::add(std::shared_ptr<Object> expression)
{
    m_expressions.push_back(std::move(expression));
}


Number::Number(std::string_view data)
{
    m_data = std::stod(data.data());
}

Number::Number(double data)
    : m_data(data)
{ }

double Number::value() const noexcept
{
    return m_data;
}

bool Number::same_with(std::shared_ptr<Object> other) const noexcept
{
    if (auto derived = std::dynamic_pointer_cast<Number>(other))
    {
        return m_data == derived->m_data;
    }
    else {
        return false;
    }
}

String::String(std::string data)
    : m_data(std::move(data))
{ }

std::string String::value() const noexcept
{
    return m_data;
}

bool String::same_with(std::shared_ptr<Object> other) const noexcept
{
    if (auto derived = std::dynamic_pointer_cast<String>(other))
    {
        return m_data == derived->m_data;
    }
    else {
        return false;
    }
}


Symbol::Symbol(std::string_view name)
    : m_name(name)
{ }

std::string Symbol::name() const noexcept
{
    return m_name;
}

bool Symbol::same_with(std::shared_ptr<Object> other) const noexcept
{
    if (auto derived = std::dynamic_pointer_cast<Symbol>(other))
    {
        return m_name == derived->m_name;
    }
    else {
        return false;
    }
}


Unary::Unary(lexeme_t type, std::shared_ptr<Object> operation)
    : m_type(type)
    , m_operation(std::move(operation))
{ }

std::shared_ptr<Object> Unary::operand() const noexcept
{
    return m_operation;
}

lexeme_t Unary::type() const noexcept
{
    return m_type;
}

bool Unary::same_with(std::shared_ptr<Object> other) const noexcept
{
    if (auto derived = std::dynamic_pointer_cast<Unary>(other))
    {
        return m_type == derived->m_type && m_operation->same_with(derived->m_operation);
    }
    else {
        return false;
    }
}


Binary::Binary(lexeme_t type, std::shared_ptr<Object> lhs, std::shared_ptr<Object> rhs)
    : m_type(type)
    , m_lhs(std::move(lhs))
    , m_rhs(std::move(rhs))
{ }

std::shared_ptr<Object> Binary::lhs() const noexcept
{
    return m_lhs;
}

std::shared_ptr<Object> Binary::rhs() const noexcept
{
    return m_rhs;
}

lexeme_t Binary::type() const noexcept
{
    return m_type;
}

bool Binary::same_with(std::shared_ptr<Object> other) const noexcept
{
    if (auto derived = std::dynamic_pointer_cast<Binary>(other))
    {
        return m_type == derived->m_type && m_lhs->same_with(derived->m_lhs) && m_rhs->same_with(derived->m_rhs);
    }
    else {
        return false;
    }
}


Block::Block(std::vector<std::shared_ptr<Object>> statements)
    : m_statements(std::move(statements))
{ }

std::vector<std::shared_ptr<Object>> Block::statements()
{
    return m_statements;
}

bool Block::same_with(std::shared_ptr<Object> other) const noexcept
{
    if (auto derived = std::dynamic_pointer_cast<Block>(other))
    {
        if (m_statements.size() != derived->m_statements.size())
            return false;

        for (std::size_t i = 0; i < m_statements.size(); i++)
        {
            if (!m_statements[i]->same_with(derived->m_statements[i]))
                return false;
        }

        return true;
    }
    else {
        return false;
    }
}


While::While(std::shared_ptr<Object> exit_condition, std::shared_ptr<Block> block)
    : m_exit_condition(std::move(exit_condition))
    , m_block(std::move(block))
{ }

std::shared_ptr<Object> While::exit_condition() const noexcept
{
    return m_exit_condition;
}

std::shared_ptr<Block> While::body() const noexcept
{
    return m_block;
}

bool While::same_with(std::shared_ptr<Object> other) const noexcept
{
    if (auto derived = std::dynamic_pointer_cast<While>(other))
    {
        return m_exit_condition->same_with(derived->m_exit_condition) && m_block->same_with(derived->m_block);
    }
    else {
        return false;
    }
}


void For::set_init(std::shared_ptr<Object> init)
{
    m_for_init = std::move(init);
}

void For::set_exit_condition(std::shared_ptr<Object> exit_condition)
{
    m_for_exit_condition = std::move(exit_condition);
}

void For::set_increment(std::shared_ptr<Object> increment)
{
    m_for_increment = std::move(increment);
}

void For::set_body(std::shared_ptr<Block> block)
{
    m_block = std::move(block);
}

std::shared_ptr<Object> For::loop_init() const noexcept
{
    return m_for_init;
}

std::shared_ptr<Object> For::exit_condition() const noexcept
{
    return m_for_exit_condition;
}

std::shared_ptr<Object> For::increment() const noexcept
{
    return m_for_increment;
}

std::shared_ptr<Block> For::body() const noexcept
{
    return m_block;
}

bool For::same_with(std::shared_ptr<Object> other) const noexcept
{
    if (auto derived = std::dynamic_pointer_cast<For>(other))
    {
        bool is_same = true;

        if (m_for_init && derived->m_for_init)
            is_same &= m_for_init->same_with(derived->m_for_init);

        if (m_for_exit_condition && derived->m_for_exit_condition)
            is_same &= m_for_exit_condition->same_with(derived->m_for_exit_condition);

        if (m_for_increment && derived->m_for_increment)
            is_same &= m_for_increment->same_with(derived->m_for_increment);

        return is_same;
    }
    else {
        return false;
    }
}


If::If(std::shared_ptr<Object> exit_condition, std::shared_ptr<Block> body)
    : m_exit_condition(std::move(exit_condition))
    , m_body(std::move(body))
{ }

If::If(std::shared_ptr<Object> exit_condition, std::shared_ptr<Block> body, std::shared_ptr<Block> else_body)
    : m_exit_condition(std::move(exit_condition))
    , m_body(std::move(body))
    , m_else_body(std::move(else_body))
{ }

std::shared_ptr<Object> If::condition() const noexcept
{
    return m_exit_condition;
}

std::shared_ptr<Object> If::body() const noexcept
{
    return m_body;
}

std::shared_ptr<Object> If::else_body() const noexcept
{
    return m_else_body;
}

bool If::same_with(std::shared_ptr<Object> other) const noexcept
{
    if (auto derived = std::dynamic_pointer_cast<If>(other))
    {
        if (!m_else_body && !derived->m_else_body)
        {
            return m_exit_condition->same_with(derived->m_exit_condition)
                   && m_body->same_with(derived->m_body);
        }
        else if (m_else_body && derived->m_else_body) {

            return m_exit_condition->same_with(derived->m_exit_condition)
                   && m_body->same_with(derived->m_body)
                   && m_else_body->same_with(derived->m_else_body);
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }
}


Function::Function(std::string name, std::vector<std::shared_ptr<Object>> arguments, std::shared_ptr<Block> body)
    : m_name(std::move(name))
    , m_arguments(std::move(arguments))
    , m_body(std::move(body))
{ }

std::string Function::name() const noexcept
{
    return m_name;
}

std::vector<std::shared_ptr<Object>> Function::arguments() const noexcept
{
    return m_arguments;
}

std::shared_ptr<Block> Function::body() const noexcept
{
    return m_body;
}

bool Function::same_with(std::shared_ptr<Object> other) const noexcept
{
    if (auto other_function = std::dynamic_pointer_cast<Function>(other))
    {
        if (m_name != other_function->m_name)
            return false;

        if (m_arguments.size() != other_function->m_arguments.size())
            return false;

        if (!m_body->same_with(other_function->m_body))
            return false;

        for (std::size_t i = 0; i < m_arguments.size(); i++)
        {
            if (!m_arguments[i]->same_with(other_function->m_arguments[i]))
                return false;
        }

        return true;
    }
    else {
        return false;
    }
}


FunctionCall::FunctionCall(std::string name, std::vector<std::shared_ptr<Object>> arguments)
    : m_name(std::move(name))
    , m_arguments(std::move(arguments))
{ }

std::string FunctionCall::name() const noexcept
{
    return m_name;
}

std::vector<std::shared_ptr<Object>> FunctionCall::arguments() const noexcept
{
    return m_arguments;
}

bool FunctionCall::same_with(std::shared_ptr<Object> other) const noexcept
{
    if (auto function_call = std::dynamic_pointer_cast<FunctionCall>(other))
    {
        if (m_name != function_call->m_name)
            return false;

        if (m_arguments.size() != function_call->m_arguments.size())
            return false;

        for (std::size_t i = 0; i < m_arguments.size(); i++)
        {
            if (!m_arguments[i]->same_with(function_call->m_arguments[i]))
                return false;
        }

        return true;
    }
    else {
        return false;
    }
}

} // namespace ast