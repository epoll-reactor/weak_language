#include "../../include/ast/ast.hpp"

namespace ast {

TypeFieldOperator::TypeFieldOperator(std::string type_name, std::string type_field) noexcept(true)
    : m_type_name(std::move(type_name))
    , m_type_field(std::move(type_field)) {}

const std::string& TypeFieldOperator::name() const noexcept(true)
{
    return m_type_name;
}

const std::string& TypeFieldOperator::field() const noexcept(true)
{
    return m_type_field;
}

} // namespace ast