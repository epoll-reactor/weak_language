#include "../../include/parser/ast.hpp"

namespace ast {

std::vector<std::shared_ptr<Object>> RootObject::get()
{
    return m_expressions;
}

void RootObject::add(std::shared_ptr<Object> expression)
{
    m_expressions.push_back(std::move(expression));
}
} // namespace ast