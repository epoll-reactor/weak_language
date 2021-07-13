#include <utility>

#include "../../include/ast/ast.hpp"

namespace ast {

While::While(boost::local_shared_ptr<Object> exit_condition, boost::local_shared_ptr<Block> block)
    : m_exit_condition(std::move(exit_condition))
    , m_block(std::move(block))
{ }

const boost::local_shared_ptr<Object>& While::exit_condition() const noexcept
{
    return m_exit_condition;
}

const boost::local_shared_ptr<Block>& While::body() const noexcept
{
    return m_block;
}

} // namespace ast
