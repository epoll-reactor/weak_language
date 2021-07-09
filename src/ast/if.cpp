#include "../../include/parser/ast.hpp"

namespace ast {

If::If(std::weak_ptr<Object> exit_condition, std::weak_ptr<Block> body)
    : m_exit_condition(exit_condition)
    , m_body(body)
{ }

If::If(std::weak_ptr<Object> exit_condition, std::weak_ptr<Block> body, std::weak_ptr<Block> else_body)
    : m_exit_condition(exit_condition)
    , m_body(body)
    , m_else_body(else_body)
{ }

const std::shared_ptr<Object>& If::condition() const noexcept
{
    return m_exit_condition;
}

const std::shared_ptr<Block>& If::body() const noexcept
{
    return m_body;
}

const std::shared_ptr<Block>& If::else_body() const noexcept
{
    return m_else_body;
}

ast_type_t If::ast_type() const noexcept
{
    return ast_type_t::IF;
}
} // namespace ast