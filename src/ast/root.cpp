#include "../../include/ast/ast.hpp"

namespace ast {

const std::vector<boost::local_shared_ptr<Object>>& RootObject::get()
{
    return m_expressions;
}

void RootObject::add(boost::local_shared_ptr<Object> expression)
{
    m_expressions.push_back(std::move(expression));
}
} // namespace ast
