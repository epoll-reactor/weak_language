#include "../../include/parser/ast.hpp"

namespace ast {

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

ast_type_t Binary::ast_type() const noexcept
{
    return ast_type_t::BINARY;
}
} // namespace ast