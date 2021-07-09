#include "../../include/parser/ast.hpp"

namespace ast {

While::While(std::weak_ptr<Object> exit_condition, std::weak_ptr<Block> block)
    : m_exit_condition(exit_condition)
    , m_block(block)
{ }

const std::shared_ptr<Object>& While::exit_condition() const noexcept
{
    return m_exit_condition;
}

const std::shared_ptr<Block>& While::body() const noexcept
{
    return m_block;
}

ast_type_t While::ast_type() const noexcept
{
    return ast_type_t::WHILE;
}

} // namespace ast