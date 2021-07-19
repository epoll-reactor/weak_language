#include "../../include/ast/ast.hpp"

namespace ast {

If::If(boost::local_shared_ptr<Object> exit_condition, boost::local_shared_ptr<Block> body) noexcept(true)
    : m_exit_condition(std::move(exit_condition))
    , m_body(std::move(body)) {}

If::If(boost::local_shared_ptr<Object> exit_condition, boost::local_shared_ptr<Block> body, boost::local_shared_ptr<Block> else_body) noexcept(true)
    : m_exit_condition(std::move(exit_condition))
    , m_body(std::move(body))
    , m_else_body(std::move(else_body)) {}

const boost::local_shared_ptr<Object>& If::condition() const noexcept(true)
{
    return m_exit_condition;
}

const boost::local_shared_ptr<Block>& If::body() const noexcept(true)
{
    return m_body;
}

const boost::local_shared_ptr<Block>& If::else_body() const noexcept(true)
{
    return m_else_body;
}

} // namespace ast
