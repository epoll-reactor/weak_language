#include "../../include/ast/ast.hpp"

namespace ast {

ArraySubscriptOperator::ArraySubscriptOperator(std::string_view name, boost::intrusive_ptr<Object> index)
    : m_name(name)
    , m_index(index)
{ }

const std::string& ArraySubscriptOperator::symbol_name() const noexcept
{
    return m_name;
}

const boost::intrusive_ptr<Object>& ArraySubscriptOperator::index() const noexcept
{
    return m_index;
}

ast_type_t ArraySubscriptOperator::ast_type() const noexcept
{
    return ast_type_t::ARRAY_SUBSCRIPT_OPERATOR;
}
} // namespace ast
