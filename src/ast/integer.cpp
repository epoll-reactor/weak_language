#include "../../include/ast/ast.hpp"

namespace ast {

Integer::Integer(std::string_view data) noexcept(false)
    : m_data(std::stoi(data.data())) {}

Integer::Integer(int32_t data) noexcept(true)
    : m_data(data) {}

int32_t& Integer::value() noexcept(true)
{
    return m_data;
}

const int32_t& Integer::value() const noexcept(true)
{
    return m_data;
}

} // namespace ast
