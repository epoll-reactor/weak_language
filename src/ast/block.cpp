#include "../../include/ast/ast.hpp"

namespace ast {

Block::Block(std::vector<boost::local_shared_ptr<Object>> statements) noexcept(true)
    : m_statements(std::move(statements)) {}

const std::vector<boost::local_shared_ptr<Object>>& Block::statements() const noexcept(true)
{
    return m_statements;
}

std::vector<boost::local_shared_ptr<Object>>& Block::statements() noexcept(false)
{
    return m_statements;
}

} // namespace ast
