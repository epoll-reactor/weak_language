#include <utility>

#include "../../include/ast/ast.hpp"

namespace ast {

Function::Function(std::string name, std::vector<boost::local_shared_ptr<Object>> arguments, boost::local_shared_ptr<Block> body) noexcept(true)
    : m_name(std::move(name))
    , m_arguments(std::move(arguments))
    , m_body(std::move(body)) {}

std::string Function::name() const noexcept(true)
{
    return m_name;
}

const std::vector<boost::local_shared_ptr<Object>>& Function::arguments() const noexcept(true)
{
    return m_arguments;
}

const boost::local_shared_ptr<Block>& Function::body() const noexcept(true)
{
    return m_body;
}

} // namespace ast
