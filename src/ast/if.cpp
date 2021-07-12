#include "../../include/ast/ast.hpp"

namespace ast {

If::If(boost::intrusive_ptr<Object> exit_condition, boost::intrusive_ptr<Block> body)
    : m_exit_condition(exit_condition)
    , m_body(body)
{ }

If::If(boost::intrusive_ptr<Object> exit_condition, boost::intrusive_ptr<Block> body, boost::intrusive_ptr<Block> else_body)
    : m_exit_condition(exit_condition)
    , m_body(body)
    , m_else_body(else_body)
{ }

const boost::intrusive_ptr<Object>& If::condition() const noexcept
{
    return m_exit_condition;
}

const boost::intrusive_ptr<Block>& If::body() const noexcept
{
    return m_body;
}

const boost::intrusive_ptr<Block>& If::else_body() const noexcept
{
    return m_else_body;
}

ast_type_t If::ast_type() const noexcept
{
    return ast_type_t::IF;
}
} // namespace ast
