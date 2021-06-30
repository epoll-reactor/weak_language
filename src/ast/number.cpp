#include "../../include/parser/ast.hpp"

namespace ast {

Number::Number(std::string_view data)
{
    m_data = std::stod(data.data());
}

Number::Number(double data)
    : m_data(data)
{ }

double Number::value() const noexcept
{
    return m_data;
}
} // namespace ast