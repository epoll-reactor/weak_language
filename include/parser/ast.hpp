#ifndef PARSE_OBJECT_HPP
#define PARSE_OBJECT_HPP

#include <memory>
#include <vector>

#include "../lexer/lexeme.hpp"


namespace ast {

class Object
{
public:
    virtual ~Object() = default;
    virtual bool same_with(std::shared_ptr<Object> other) const noexcept = 0;
};

class RootObject
{
public:
    std::vector<std::shared_ptr<Object>> get();

    void add(std::shared_ptr<Object> expression);

private:
    std::vector<std::shared_ptr<Object>> m_expressions;
};

class Number : public Object
{
public:
    Number(std::string_view data);

    Number(double data);

    double value() const noexcept;

    bool same_with(std::shared_ptr<Object> other) const noexcept override;

private:
    double m_data;
};

class String : public Object
{
public:
    String(std::string data);

    std::string value() const noexcept;

    bool same_with(std::shared_ptr<Object> other) const noexcept override;

private:
    std::string m_data;
};

class Symbol : public Object
{
public:
    Symbol(std::string_view name);

    std::string name() const noexcept;

    bool same_with(std::shared_ptr<Object> other) const noexcept override;

private:
    std::string m_name;
};

class Unary : public Object
{
public:
    Unary(lexeme_t type, std::shared_ptr<Object> operation);

    std::shared_ptr<Object> operand() const noexcept;

    lexeme_t type() const noexcept;

    bool same_with(std::shared_ptr<Object> other) const noexcept override;

private:
    lexeme_t m_type;
    std::shared_ptr<Object> m_operation;
};

class Binary : public Object
{
public:
    Binary(lexeme_t type, std::shared_ptr<Object> lhs, std::shared_ptr<Object> rhs);

    std::shared_ptr<Object> lhs() const noexcept;

    std::shared_ptr<Object> rhs() const noexcept;

    lexeme_t type() const noexcept;

    bool same_with(std::shared_ptr<Object> other) const noexcept override;

private:
    lexeme_t m_type;
    std::shared_ptr<Object> m_lhs;
    std::shared_ptr<Object> m_rhs;
};

class Block : public Object
{
public:
    Block(std::vector<std::shared_ptr<Object>> statements);

    std::vector<std::shared_ptr<Object>> statements();

    bool same_with(std::shared_ptr<Object> other) const noexcept override;

private:
    std::vector<std::shared_ptr<Object>> m_statements;
};

class While : public Object
{
public:
    While(std::shared_ptr<Object> exit_condition, std::shared_ptr<Block> block);

    std::shared_ptr<Object> exit_condition() const noexcept;

    std::shared_ptr<Block> body() const noexcept;

    bool same_with(std::shared_ptr<Object> other) const noexcept override;

private:
    std::shared_ptr<Object> m_exit_condition;
    std::shared_ptr<Block> m_block;
};

class For : public Object
{
public:
    For() = default;

    void set_init(std::shared_ptr<Object> init);

    void set_exit_condition(std::shared_ptr<Object> exit_condition);

    void set_increment(std::shared_ptr<Object> increment);

    void set_body(std::shared_ptr<Block> block);

    std::shared_ptr<Object> loop_init() const noexcept;

    std::shared_ptr<Object> exit_condition() const noexcept;

    std::shared_ptr<Object> increment() const noexcept;

    std::shared_ptr<Block> body() const noexcept;

    bool same_with(std::shared_ptr<Object> other) const noexcept override;

private:
    std::shared_ptr<Object> m_for_init;
    std::shared_ptr<Object> m_for_exit_condition;
    std::shared_ptr<Object> m_for_increment;
    std::shared_ptr<Block> m_block;
};

class If : public Object
{
public:
    If(std::shared_ptr<Object> exit_condition, std::shared_ptr<Block> body);

    If(std::shared_ptr<Object> exit_condition, std::shared_ptr<Block> body, std::shared_ptr<Block> else_body);

    std::shared_ptr<Object> condition() const noexcept;

    std::shared_ptr<Object> body() const noexcept;

    std::shared_ptr<Object> else_body() const noexcept;

    bool same_with(std::shared_ptr<Object> other) const noexcept override;

private:
    std::shared_ptr<Object> m_exit_condition;
    std::shared_ptr<Block> m_body;
    std::shared_ptr<Block> m_else_body;
};

class Function : public Object
{
public:
    Function(std::string name, std::vector<std::shared_ptr<Object>> arguments, std::shared_ptr<Block> body);

    std::string name() const noexcept;

    std::vector<std::shared_ptr<Object>> arguments() const noexcept;

    std::shared_ptr<Block> body() const noexcept;

    bool same_with(std::shared_ptr<Object> other) const noexcept override;

private:
    std::string m_name;
    std::vector<std::shared_ptr<Object>> m_arguments;
    std::shared_ptr<Block> m_body;
};

class FunctionCall : public Object
{
public:
    FunctionCall(std::string name, std::vector<std::shared_ptr<Object>> arguments);

    std::string name() const noexcept;

    std::vector<std::shared_ptr<Object>> arguments() const noexcept;

    bool same_with(std::shared_ptr<Object> other) const noexcept override;

private:
    std::string m_name;
    std::vector<std::shared_ptr<Object>> m_arguments;
};

} // namespace ast

#endif // PARSE_OBJECT_HPP
