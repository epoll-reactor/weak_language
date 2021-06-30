#include "../../include/parser/ast.hpp"

namespace ast {

Block::Block(std::vector<std::shared_ptr<Object>> statements)
    : m_statements(std::move(statements))
{ }

const std::vector<std::shared_ptr<Object>>& Block::statements()
{
    return m_statements;
}
} // namespace ast