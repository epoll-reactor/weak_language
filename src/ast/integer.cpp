#include "../../include/parser/ast.hpp"

namespace ast {

Integer::Integer(std::string_view data)
{
    m_data = std::stoi(data.data());
}

Integer::Integer(int32_t data)
    : m_data(data)
{ }

int32_t Integer::value() const noexcept
{
    return m_data;
}

ast_type_t Integer::ast_type() const noexcept
{
    return ast_type_t::INTEGER;
}
} // namespace ast