#include "../../include/parser/ast.hpp"

namespace ast {

const std::vector<boost::intrusive_ptr<Object>>& RootObject::get()
{
    return m_expressions;
}

void RootObject::add(boost::intrusive_ptr<Object> expression)
{
    m_expressions.push_back(std::move(expression));
}
} // namespace ast
