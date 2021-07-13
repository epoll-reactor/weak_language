#include "../../include/ast/ast.hpp"

namespace ast {

Binary::Binary(token_t type, boost::local_shared_ptr<Object> lhs, boost::local_shared_ptr<Object> rhs)
    : m_type(type)
    , m_lhs(std::move(lhs))
    , m_rhs(std::move(rhs))
{ }

const boost::local_shared_ptr<Object>& Binary::lhs() const noexcept
{
    return m_lhs;
}

const boost::local_shared_ptr<Object>& Binary::rhs() const noexcept
{
    return m_rhs;
}

token_t Binary::type() const noexcept
{
    return m_type;
}

} // namespace ast
