#include "../../include/ast/ast.hpp"

namespace ast {

TypeDefinition::TypeDefinition(std::string_view name, std::vector<std::string> fields) noexcept(true)
    : m_name(name)
    , m_fields(std::move(fields)) {}

const std::string& TypeDefinition::name() const noexcept(true)
{
    return m_name;
}

const std::vector<std::string>& TypeDefinition::fields() const noexcept(true)
{
    return m_fields;
}

} // namespace ast
