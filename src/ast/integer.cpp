#include "../../include/ast/ast.hpp"

namespace ast {

Integer::Integer(std::string_view data)
{
    m_data = std::stoi(data.data());
}

Integer::Integer(int32_t data)
    : m_data(data)
{ }

int32_t& Integer::value() noexcept
{
    return m_data;
}

const int32_t& Integer::value() const noexcept
{
    return m_data;
}

} // namespace ast
