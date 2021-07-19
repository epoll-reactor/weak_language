#include <utility>

#include "../../include/ast/ast.hpp"

namespace ast {

Unary::Unary(token_t type, boost::local_shared_ptr<Object> operation) noexcept(true)
    : m_type(type)
    , m_operation(std::move(operation) {}

boost::local_shared_ptr<Object> Unary::operand() const noexcept(true)
{
    return m_operation;
}

token_t Unary::type() const noexcept(true)
{
    return m_type;
}

} // namespace ast
