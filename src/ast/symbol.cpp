#include "../../include/parser/ast.hpp"

namespace ast {

Symbol::Symbol(std::string_view name)
    : m_name(name)
{ }

std::string Symbol::name() const noexcept
{
    return m_name;
}
} // namespace ast