#include "../../include/ast/ast.hpp"

namespace ast {

Function::Function(std::string_view name, std::vector<boost::intrusive_ptr<Object>> arguments, boost::intrusive_ptr<Block> body)
    : m_name(name)
    , m_arguments(std::move(arguments))
    , m_body(std::move(body))
{ }

std::string Function::name() const noexcept
{
    return m_name;
}

const std::vector<boost::intrusive_ptr<Object>>& Function::arguments() const noexcept
{
    return m_arguments;
}

const boost::intrusive_ptr<Block>& Function::body() const noexcept
{
    return m_body;
}

ast_type_t Function::ast_type() const noexcept
{
    return ast_type_t::FUNCTION;
}
} // namespace ast
