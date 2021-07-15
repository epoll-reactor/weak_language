#include "../../include/ast/ast.hpp"

namespace ast {

Float::Float(std::string_view data) noexcept(false)
{
    m_data = std::stod(data.data());
}

Float::Float(double data) noexcept(true)
    : m_data(data)
{ }

double& Float::value() noexcept(true)
{
    return m_data;
}

const double& Float::value() const noexcept(true)
{
    return m_data;
}

} // namespace ast
