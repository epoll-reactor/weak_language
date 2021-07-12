#ifndef WEAK_AST_HPP
#define WEAK_AST_HPP

#include <boost/smart_ptr/local_shared_ptr.hpp>
#include <boost/smart_ptr/make_local_shared.hpp>
#include <memory>
#include <vector>

#include "../lexer/lexeme.hpp"

namespace ast {

enum struct ast_type_t { OBJECT, INTEGER, FLOAT, STRING, SYMBOL, ARRAY, ARRAY_SUBSCRIPT_OPERATOR, UNARY, BINARY, BLOCK, WHILE, FOR, IF, FUNCTION, FUNCTION_CALL, TYPE_DEFINITION };

class Object
{
public:
    virtual ast_type_t ast_type() const noexcept { return ast_type_t::OBJECT; }
    virtual ~Object() = default;

    size_t& reference_count() noexcept { return m_reference_counter; }

private:
    size_t m_reference_counter;
};

class RootObject
{
public:
    const std::vector<boost::local_shared_ptr<Object>>& get();

    void add(boost::local_shared_ptr<Object> expression);

private:
    std::vector<boost::local_shared_ptr<Object>> m_expressions;
};

class Integer : public Object
{
public:
    Integer(std::string_view data);

    Integer(int32_t data);

    int32_t& value() noexcept;

    const int32_t& value() const noexcept;

    ast_type_t ast_type() const noexcept override final;

private:
    int32_t m_data;
};

class Float : public Object
{
public:
    Float(std::string_view data);

    Float(double data);

    double& value() noexcept;

    const double& value() const noexcept;

    ast_type_t ast_type() const noexcept override final;

private:
    double m_data;
};

class String : public Object
{
public:
    String(std::string data);

    const std::string& value() const noexcept;

    ast_type_t ast_type() const noexcept override final;

private:
    std::string m_data;
};

class Symbol : public Object
{
public:
    Symbol(std::string_view name);

    const std::string& name() const noexcept;

    ast_type_t ast_type() const noexcept override final;

private:
    std::string m_name;
};

class Array : public Object
{
public:
    Array(std::vector<boost::local_shared_ptr<Object>> elements);

    std::vector<boost::local_shared_ptr<Object>>& elements() noexcept;

    ast_type_t ast_type() const noexcept override final;

private:
    std::vector<boost::local_shared_ptr<Object>> m_elements;
};

class ArraySubscriptOperator : public Object
{
public:
    ArraySubscriptOperator(std::string_view name, boost::local_shared_ptr<Object> index);

    const std::string& symbol_name() const noexcept;

    const boost::local_shared_ptr<Object>& index() const noexcept;

    ast_type_t ast_type() const noexcept override final;

private:
    std::string m_name;
    boost::local_shared_ptr<Object> m_index;
};

class Unary : public Object
{
public:
    Unary(lexeme_t type, boost::local_shared_ptr<Object> operation);

    boost::local_shared_ptr<Object> operand() const noexcept;

    lexeme_t type() const noexcept;

    ast_type_t ast_type() const noexcept override final;

private:
    lexeme_t m_type;
    boost::local_shared_ptr<Object> m_operation;
};

class Binary : public Object
{
public:
    Binary(lexeme_t type, boost::local_shared_ptr<Object> lhs, boost::local_shared_ptr<Object> rhs);

    const boost::local_shared_ptr<Object>& lhs() const noexcept;

    const boost::local_shared_ptr<Object>& rhs() const noexcept;

    lexeme_t type() const noexcept;

    ast_type_t ast_type() const noexcept override final;

private:
    lexeme_t m_type;
    boost::local_shared_ptr<Object> m_lhs;
    boost::local_shared_ptr<Object> m_rhs;
};

class Block : public Object
{
public:
    Block(std::vector<boost::local_shared_ptr<Object>> statements);

    const std::vector<boost::local_shared_ptr<Object>>& statements();

    ast_type_t ast_type() const noexcept override final;

private:
    std::vector<boost::local_shared_ptr<Object>> m_statements;
};

class While : public Object
{
public:
    While(boost::local_shared_ptr<Object> exit_condition, boost::local_shared_ptr<Block> block);

    const boost::local_shared_ptr<Object>& exit_condition() const noexcept;

    const boost::local_shared_ptr<Block>& body() const noexcept;

    ast_type_t ast_type() const noexcept override final;

private:
    boost::local_shared_ptr<Object> m_exit_condition;
    boost::local_shared_ptr<Block> m_block;
};

class For : public Object
{
public:
    For() = default;

    void set_init(boost::local_shared_ptr<Object> init);

    void set_exit_condition(boost::local_shared_ptr<Object> exit_condition);

    void set_increment(boost::local_shared_ptr<Object> increment);

    void set_body(boost::local_shared_ptr<Block> block);

    const boost::local_shared_ptr<Object>& loop_init() const noexcept;

    const boost::local_shared_ptr<Object>& exit_condition() const noexcept;

    const boost::local_shared_ptr<Object>& increment() const noexcept;

    const boost::local_shared_ptr<Block>& body() const noexcept;

    ast_type_t ast_type() const noexcept override final;

private:
    boost::local_shared_ptr<Object> m_for_init;
    boost::local_shared_ptr<Object> m_for_exit_condition;
    boost::local_shared_ptr<Object> m_for_increment;
    boost::local_shared_ptr<Block> m_block;
};

class If : public Object
{
public:
    If(boost::local_shared_ptr<Object> exit_condition, boost::local_shared_ptr<Block> body);

    If(boost::local_shared_ptr<Object> exit_condition, boost::local_shared_ptr<Block> body, boost::local_shared_ptr<Block> else_body);

    const boost::local_shared_ptr<Object>& condition() const noexcept;

    const boost::local_shared_ptr<Block>& body() const noexcept;

    const boost::local_shared_ptr<Block>& else_body() const noexcept;

    ast_type_t ast_type() const noexcept override final;

private:
    boost::local_shared_ptr<Object> m_exit_condition;
    boost::local_shared_ptr<Block> m_body;
    boost::local_shared_ptr<Block> m_else_body;
};

class Function : public Object
{
public:
    Function(std::string_view name, std::vector<boost::local_shared_ptr<Object>> arguments, boost::local_shared_ptr<Block> body);

    std::string name() const noexcept;

    const std::vector<boost::local_shared_ptr<Object>>& arguments() const noexcept;

    const boost::local_shared_ptr<Block>& body() const noexcept;

    ast_type_t ast_type() const noexcept override final;

private:
    std::string m_name;
    std::vector<boost::local_shared_ptr<Object>> m_arguments;
    boost::local_shared_ptr<Block> m_body;
};

class FunctionCall : public Object
{
public:
    FunctionCall(std::string name, std::vector<boost::local_shared_ptr<Object>> arguments);

    const std::string& name() const noexcept;

    const std::vector<boost::local_shared_ptr<Object>>& arguments() const noexcept;

    ast_type_t ast_type() const noexcept override final;

private:
    std::string m_name;
    std::vector<boost::local_shared_ptr<Object>> m_arguments;
};

class TypeDefinition : public Object
{
public:
    TypeDefinition(std::string_view name, std::vector<std::string> fields);

    const std::string& name() const noexcept;
    const std::vector<std::string>& fields() const noexcept;

    ast_type_t ast_type() const noexcept override final;

private:
    std::string m_name;
    std::vector<std::string> m_fields;
};

inline void local_shared_ptr_release(Object* o) noexcept
{
    if (--o->reference_count() == 0) { delete o; }
}

inline void local_shared_ptr_add_ref(Object* o) noexcept
{
    ++o->reference_count();
}

} // namespace ast

#endif // WEAK_AST_HPP
