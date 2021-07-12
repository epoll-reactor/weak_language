#include "../../include/ast/ast.hpp"

namespace ast {

Binary::Binary(lexeme_t type, boost::local_shared_ptr<Object> lhs, boost::local_shared_ptr<Object> rhs)
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

lexeme_t Binary::type() const noexcept
{
    return m_type;
}

ast_type_t Binary::ast_type() const noexcept
{
    return ast_type_t::BINARY;
}
} // namespace ast
