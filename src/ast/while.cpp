#include "../../include/parser/ast.hpp"

namespace ast {

While::While(std::shared_ptr<Object> exit_condition, std::shared_ptr<Block> block)
    : m_exit_condition(std::move(exit_condition))
    , m_block(std::move(block))
{ }

std::shared_ptr<Object> While::exit_condition() const noexcept
{
    return m_exit_condition;
}

std::shared_ptr<Block> While::body() const noexcept
{
    return m_block;
}
} // namespace ast