#include "../../include/ast/ast.hpp"

namespace ast {

TypeObject::TypeObject(std::unordered_map<std::string, boost::local_shared_ptr<Object>> arguments) noexcept(true)
    : m_arguments(std::move(arguments)) {}

const std::unordered_map<std::string, boost::local_shared_ptr<Object>>& TypeObject::fields() const noexcept(false)
{
    return m_arguments;
}

} // namespace ast