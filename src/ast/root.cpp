#include "../../include/ast/ast.hpp"

namespace ast {

std::vector<boost::local_shared_ptr<Object>>& RootObject::get() noexcept(true)
{
    return m_expressions;
}

const std::vector<boost::local_shared_ptr<Object>>& RootObject::get() const
{
    return m_expressions;
}

void RootObject::add(boost::local_shared_ptr<Object> expression) noexcept(false)
{
    m_expressions.push_back(std::move(expression));
}

} // namespace ast
