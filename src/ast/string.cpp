#include "../../include/parser/ast.hpp"

namespace ast {

String::String(std::string data)
    : m_data(std::move(data))
{ }

std::string String::value() const noexcept
{
    return m_data;
}

} // namespace ast