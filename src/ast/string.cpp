#include "../../include/parser/ast.hpp"

namespace ast {

String::String(std::string data)
    : m_data(std::move(data))
{ }

const std::string& String::value() const noexcept
{
    return m_data;
}

ast_type_t String::ast_type() const noexcept
{
    return ast_type_t::STRING;
}
} // namespace ast