#include "../../include/parser/ast.hpp"

namespace ast {

ArraySubscriptOperator::ArraySubscriptOperator(std::string_view name, std::shared_ptr<Object> index)
    : m_name(name)
    , m_index(std::move(index))
{ }

std::string ArraySubscriptOperator::symbol_name() const noexcept
{
    return m_name;
}

std::shared_ptr<Object> ArraySubscriptOperator::index() const noexcept
{
    return m_index;
}
} // namespace ast