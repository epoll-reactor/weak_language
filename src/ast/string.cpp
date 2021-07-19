#include "../../include/ast/ast.hpp"

namespace ast {

String::String(std::string data) noexcept(true)
    : m_data(std::move(data)) {}

const std::string& String::value() const noexcept(true)
{
    return m_data;
}

} // namespace ast
