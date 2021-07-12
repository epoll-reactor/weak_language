#include <utility>

#include "../../include/ast/ast.hpp"

namespace ast {

Unary::Unary(lexeme_t type, boost::local_shared_ptr<Object> operation)
    : m_type(type)
    , m_operation(std::move(operation))
{ }

boost::local_shared_ptr<Object> Unary::operand() const noexcept
{
    return m_operation;
}

lexeme_t Unary::type() const noexcept
{
    return m_type;
}

ast_type_t Unary::ast_type() const noexcept
{
    return ast_type_t::UNARY;
}
} // namespace ast
