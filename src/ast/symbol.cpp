#include "../../include/parser/ast.hpp"

namespace ast {

Symbol::Symbol(std::string_view name)
    : m_name(name)
{ }

const std::string& Symbol::name() const noexcept
{
    return m_name;
}

ast_type_t Symbol::ast_type() const noexcept
{
    return ast_type_t::SYMBOL;
}
} // namespace ast