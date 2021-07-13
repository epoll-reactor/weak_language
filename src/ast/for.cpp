#include "../../include/ast/ast.hpp"

namespace ast {

void For::set_init(boost::local_shared_ptr<Object> init)
{
    m_for_init = std::move(init);
}

void For::set_exit_condition(boost::local_shared_ptr<Object> exit_condition)
{
    m_for_exit_condition = std::move(exit_condition);
}

void For::set_increment(boost::local_shared_ptr<Object> increment)
{
    m_for_increment = std::move(increment);
}

void For::set_body(boost::local_shared_ptr<Block> block)
{
    m_block = std::move(block);
}

const boost::local_shared_ptr<Object>& For::loop_init() const noexcept
{
    return m_for_init;
}

const boost::local_shared_ptr<Object>& For::exit_condition() const noexcept
{
    return m_for_exit_condition;
}

const boost::local_shared_ptr<Object>& For::increment() const noexcept
{
    return m_for_increment;
}

const boost::local_shared_ptr<Block>& For::body() const noexcept
{
    return m_block;
}

} // namespace ast
