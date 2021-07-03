#include "../../include/parser/ast.hpp"

namespace ast {

Float::Float(std::string_view data)
{
    m_data = std::stod(data.data());
}

Float::Float(double data)
    : m_data(data)
{ }

double Float::value() const noexcept
{
    return m_data;
}
} // namespace ast