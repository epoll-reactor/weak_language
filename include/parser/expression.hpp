#ifndef PARSE_OBJECT_HPP
#define PARSE_OBJECT_HPP

#include <string>
#include <memory>

#include "../parser/parse_error.hpp"

namespace expression {

class Object
{
public:
    virtual ~Object() = default;
    virtual bool same_with(std::shared_ptr<Object> other) const noexcept = 0;
};

class RootObject
{
public:
    auto get()
    {
        return m_expressions;
    }

    void add(std::shared_ptr<Object> expression)
    {
        m_expressions.push_back(std::move(expression));
    }

private:
    std::vector<std::shared_ptr<Object>> m_expressions;
};

class Number : public Object
{
public:
    explicit Number(std::string data)
    {
        try
        {
            m_data = std::stod(data);
        }
        catch (std::exception& convert_error) {
            throw ParseError("Correct number or floating point literal expected");
        }
    }

    double value() const noexcept
    {
        return m_data;
    }

    bool same_with(std::shared_ptr<Object> other) const noexcept override
    {
        if (auto derived = std::dynamic_pointer_cast<Number>(other))
        {
            return m_data == derived->m_data;
        }
        else {
            return false;
        }
    }

private:
    double m_data;
};

class String : public Object
{
public:
    explicit String(std::string data)
        : m_data(std::move(data))
    { }

    std::string value() const noexcept
    {
        return m_data;
    }

    bool same_with(std::shared_ptr<Object> other) const noexcept override
    {
        if (auto derived = std::dynamic_pointer_cast<String>(other))
        {
            return m_data == derived->m_data;
        }
        else {
            return false;
        }
    }

private:
    std::string m_data;
};

class Symbol : public Object
{
public:
    Symbol(std::string_view name)
        : m_name(name)
    { }

    std::string name() const noexcept
    {
        return m_name;
    }

    bool same_with(std::shared_ptr<Object> other) const noexcept override
    {
        if (auto derived = std::dynamic_pointer_cast<Symbol>(other))
        {
            return m_name == derived->m_name;
        }
        else {
            return false;
        }
    }

private:
    std::string m_name;
};

class Unary : public Object
{
public:
    Unary(lexeme_t type, std::shared_ptr<Object> operation)
        : m_type(type)
        , m_operation(operation)
    { }

    std::shared_ptr<Object> operand() const noexcept
    {
        return m_operation;
    }

    lexeme_t type() const noexcept
    {
        return m_type;
    }

    bool same_with(std::shared_ptr<Object> other) const noexcept override
    {
        if (auto derived = std::dynamic_pointer_cast<Unary>(other))
        {
            return m_type == derived->m_type && m_operation->same_with(derived->m_operation);
        }
        else {
            return false;
        }
    }

private:
    lexeme_t m_type;
    std::shared_ptr<Object> m_operation;
};

class Binary : public Object
{
public:
    Binary(lexeme_t type, std::shared_ptr<Object> lhs, std::shared_ptr<Object> rhs)
        : m_type(type)
        , m_lhs(std::move(lhs))
        , m_rhs(std::move(rhs))
    { }

    std::shared_ptr<Object> lhs() const noexcept
    {
        return m_lhs;
    }

    std::shared_ptr<Object> rhs() const noexcept
    {
        return m_rhs;
    }

    lexeme_t type() const noexcept
    {
        return m_type;
    }

    bool same_with(std::shared_ptr<Object> other) const noexcept override
    {
        if (auto derived = std::dynamic_pointer_cast<Binary>(other))
        {
            return m_type == derived->m_type && m_lhs->same_with(derived->m_lhs) && m_rhs->same_with(derived->m_rhs);
        }
        else {
            return false;
        }
    }

private:
    lexeme_t m_type;
    std::shared_ptr<Object> m_lhs;
    std::shared_ptr<Object> m_rhs;
};

class Block : public Object
{
public:
    Block(std::vector<std::shared_ptr<Object>> statements)
        : m_statements(statements)
    { }

    std::vector<std::shared_ptr<Object>> statements()
    {
        return m_statements;
    }

    bool same_with(std::shared_ptr<Object> other) const noexcept override
    {
        if (auto derived = std::dynamic_pointer_cast<Block>(other))
        {
            if (m_statements.size() != derived->m_statements.size())
            {
                return false;
            }

            for (std::size_t i = 0; i < m_statements.size(); i++)
            {
                if (!m_statements[i]->same_with(derived->m_statements[i]))
                {
                    return false;
                }
            }

            return true;
        }
        else {
            return false;
        }
    }

private:
    std::vector<std::shared_ptr<Object>> m_statements;
};

class While : public Object
{
public:
    While(std::shared_ptr<Object> exit_condition, std::shared_ptr<Block> block)
        : m_exit_condition(std::move(exit_condition))
        , m_block(std::move(block))
    { }

    std::shared_ptr<Object> exit_condition() const noexcept
    {
        return m_exit_condition;
    }

    std::shared_ptr<Block> body() const noexcept
    {
        return m_block;
    }

    bool same_with(std::shared_ptr<Object> other) const noexcept override
    {
        if (auto derived = std::dynamic_pointer_cast<While>(other))
        {
            return m_exit_condition->same_with(derived->m_exit_condition) && m_block->same_with(derived->m_block);
        }
        else {
            return false;
        }
    }

private:
    std::shared_ptr<Object> m_exit_condition;
    std::shared_ptr<Block> m_block;
};

class If : public Object
{
public:
    If(std::shared_ptr<Object> exit_condition, std::shared_ptr<Block> body)
        : m_exit_condition(std::move(exit_condition))
        , m_body(std::move(body))
    { }

    If(std::shared_ptr<Object> exit_condition, std::shared_ptr<Block> body, std::shared_ptr<Block> else_body)
        : m_exit_condition(std::move(exit_condition))
        , m_body(std::move(body))
        , m_else_body(std::move(else_body))
    { }

    std::shared_ptr<Object> condition() const noexcept
    {
        return m_exit_condition;
    }

    std::shared_ptr<Object> body() const noexcept
    {
        return m_body;
    }

    std::shared_ptr<Object> else_body() const noexcept
    {
        return m_else_body;
    }

    bool same_with(std::shared_ptr<Object> other) const noexcept override
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

private:
    std::shared_ptr<Object> m_exit_condition;
    std::shared_ptr<Block> m_body;
    std::shared_ptr<Block> m_else_body;
};

} // namespace expression

#endif // PARSE_OBJECT_HPP
