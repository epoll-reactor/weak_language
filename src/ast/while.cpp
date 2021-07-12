#include "../../include/ast/ast.hpp"

namespace ast {

While::While(boost::intrusive_ptr<Object> exit_condition, boost::intrusive_ptr<Block> block)
    : m_exit_condition(exit_condition)
    , m_block(block)
{ }

const boost::intrusive_ptr<Object>& While::exit_condition() const noexcept
{
    return m_exit_condition;
}

const boost::intrusive_ptr<Block>& While::body() const noexcept
{
    return m_block;
}

ast_type_t While::ast_type() const noexcept
{
    return ast_type_t::WHILE;
}

} // namespace ast
