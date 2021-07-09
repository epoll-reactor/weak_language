#include "../../include/parser/ast.hpp"

namespace ast {

void For::set_init(std::shared_ptr<Object> init)
{
    m_for_init = std::move(init);
}

void For::set_exit_condition(std::shared_ptr<Object> exit_condition)
{
    m_for_exit_condition = std::move(exit_condition);
}

void For::set_increment(std::shared_ptr<Object> increment)
{
    m_for_increment = std::move(increment);
}

void For::set_body(std::shared_ptr<Block> block)
{
    m_block = std::move(block);
}

const std::shared_ptr<Object>& For::loop_init() const noexcept
{
    return m_for_init;
}

const std::shared_ptr<Object>& For::exit_condition() const noexcept
{
    return m_for_exit_condition;
}

const std::shared_ptr<Object>& For::increment() const noexcept
{
    return m_for_increment;
}

const std::shared_ptr<Block>& For::body() const noexcept
{
    return m_block;
}

ast_type_t For::ast_type() const noexcept
{
    return ast_type_t::FOR;
}
} // namespace ast