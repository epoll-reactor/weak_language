#include "../../include/ast/ast.hpp"

namespace ast {

TypeCreator::TypeCreator(std::string name, std::vector<boost::local_shared_ptr<Object>> arguments) noexcept(true)
    : m_name(std::move(name))
    , m_arguments(std::move(arguments))
{ }

const std::string& TypeCreator::name() const noexcept(true)
{
    return m_name;
}

const std::vector<boost::local_shared_ptr<Object>>& TypeCreator::arguments() const noexcept(true)
{
    return m_arguments;
}

} // namespace ast
