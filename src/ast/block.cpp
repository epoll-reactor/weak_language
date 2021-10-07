#include "../../include/ast/ast.hpp"

namespace ast {

Block::Block(std::vector<boost::local_shared_ptr<Object>> statements) noexcept(true)
  : statements_(std::move(statements)) {}

const std::vector<boost::local_shared_ptr<Object>>& Block::statements() const noexcept(true) {
  return statements_;
}

std::vector<boost::local_shared_ptr<Object>>& Block::statements() noexcept(false) {
  return statements_;
}

}// namespace ast
