#include "../../include/ast/ast.hpp"

namespace ast {

ArraySubscriptOperator::ArraySubscriptOperator(std::string name, boost::local_shared_ptr<Object> index) noexcept(true)
    : m_name(std::move(name))
    , m_index(std::move(index)) {}

const std::string& ArraySubscriptOperator::symbol_name() const noexcept(true)
{
    return m_name;
}

const boost::local_shared_ptr<Object>& ArraySubscriptOperator::index() const noexcept(true)
{
    return m_index;
}

} // namespace ast
