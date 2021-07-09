#include "../../include/parser/ast.hpp"

namespace ast {

FunctionCall::FunctionCall(std::string name, std::vector<boost::intrusive_ptr<Object>> arguments)
    : m_name(std::move(name))
    , m_arguments(std::move(arguments))
{ }

const std::string& FunctionCall::name() const noexcept
{
    return m_name;
}

const std::vector<boost::intrusive_ptr<Object>>& FunctionCall::arguments() const noexcept
{
    return m_arguments;
}

ast_type_t FunctionCall::ast_type() const noexcept
{
    return ast_type_t::FUNCTION_CALL;
}
} // namespace ast
