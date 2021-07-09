#include "../../include/parser/ast.hpp"

namespace ast {

Binary::Binary(lexeme_t type, boost::intrusive_ptr<Object> lhs, boost::intrusive_ptr<Object> rhs)
    : m_type(type)
    , m_lhs(lhs)
    , m_rhs(rhs)
{ }

const boost::intrusive_ptr<Object>& Binary::lhs() const noexcept
{
    return m_lhs;
}

const boost::intrusive_ptr<Object>& Binary::rhs() const noexcept
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
