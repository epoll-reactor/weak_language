#include "../../include/ast/ast.hpp"

namespace ast {

Block::Block(std::vector<boost::local_shared_ptr<Object>> statements)
    : m_statements(std::move(statements))
{ }

const std::vector<boost::local_shared_ptr<Object>>& Block::statements()
{
    return m_statements;
}

ast_type_t Block::ast_type() const noexcept
{
    return ast_type_t::BLOCK;
}
} // namespace ast
