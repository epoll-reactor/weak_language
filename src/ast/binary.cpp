#include "../../include/ast/ast.hpp"

namespace ast {

Binary::Binary(token_t type, boost::local_shared_ptr<Object> lhs, boost::local_shared_ptr<Object> rhs) noexcept(true)
  : type_(type)
  , lhs_(std::move(lhs))
  , rhs_(std::move(rhs)) {}

boost::local_shared_ptr<Object>& Binary::lhs() noexcept(true) {
  return lhs_;
}

const boost::local_shared_ptr<Object>& Binary::lhs() const noexcept(true) {
  return lhs_;
}

boost::local_shared_ptr<Object>& Binary::rhs() noexcept(true) {
  return rhs_;
}

const boost::local_shared_ptr<Object>& Binary::rhs() const noexcept(true) {
  return rhs_;
}

token_t Binary::type() const noexcept(true) {
  return type_;
}

}// namespace ast
