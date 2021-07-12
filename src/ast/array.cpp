#include "../../include/ast/ast.hpp"

namespace ast {

Array::Array(std::vector<boost::intrusive_ptr<Object>> elements)
    : m_elements(std::move(elements))
{ }

std::vector<boost::intrusive_ptr<Object>>& Array::elements() noexcept
{
    return m_elements;
}

ast_type_t Array::ast_type() const noexcept
{
    return ast_type_t::ARRAY;
}
} // namespace ast
