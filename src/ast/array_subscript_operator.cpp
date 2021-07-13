#include "../../include/ast/ast.hpp"

namespace ast {

ArraySubscriptOperator::ArraySubscriptOperator(std::string_view name, boost::local_shared_ptr<Object> index)
    : m_name(name)
    , m_index(std::move(index))
{ }

const std::string& ArraySubscriptOperator::symbol_name() const noexcept
{
    return m_name;
}

const boost::local_shared_ptr<Object>& ArraySubscriptOperator::index() const noexcept
{
    return m_index;
}

} // namespace ast
