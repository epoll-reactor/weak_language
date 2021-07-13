#include "../../include/ast/ast.hpp"

namespace ast {

String::String(std::string data)
    : m_data(std::move(data))
{ }

const std::string& String::value() const noexcept
{
    return m_data;
}

} // namespace ast
