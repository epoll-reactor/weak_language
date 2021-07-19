#include "../../include/ast/ast.hpp"

namespace ast {

Symbol::Symbol(std::string name) noexcept(true)
    : m_name(std::move(name)) {}

const std::string& Symbol::name() const noexcept(true)
{
    return m_name;
}

} // namespace ast
