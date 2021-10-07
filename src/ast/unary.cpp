#include "../../include/ast/ast.hpp"

#include <utility>

namespace ast {

Unary::Unary(token_t type, boost::local_shared_ptr<Object> operation) noexcept(true)
  : type_(type)
  , operation_(std::move(operation)) {}

boost::local_shared_ptr<Object> Unary::operand() const noexcept(true) {
  return operation_;
}

token_t Unary::type() const noexcept(true) {
  return type_;
}

}// namespace ast
