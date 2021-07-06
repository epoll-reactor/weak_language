#include "../../include/parser/ast.hpp"

namespace ast {

If::If(std::shared_ptr<Object> exit_condition, std::shared_ptr<Block> body)
    : m_exit_condition(std::move(exit_condition))
    , m_body(std::move(body))
{ }

If::If(std::shared_ptr<Object> exit_condition, std::shared_ptr<Block> body, std::shared_ptr<Block> else_body)
    : m_exit_condition(std::move(exit_condition))
    , m_body(std::move(body))
    , m_else_body(std::move(else_body))
{ }

std::shared_ptr<Object> If::condition() const noexcept
{
    return m_exit_condition;
}

std::shared_ptr<Object> If::body() const noexcept
{
    return m_body;
}

std::shared_ptr<Object> If::else_body() const noexcept
{
    return m_else_body;
}

ast_type_t If::ast_type() const noexcept
{
    return ast_type_t::IF;
}
} // namespace ast