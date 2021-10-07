#include "../../include/ast/ast.hpp"

namespace ast {

std::vector<boost::local_shared_ptr<Object>>& RootObject::get() noexcept(true) {
  return expressions_;
}

const std::vector<boost::local_shared_ptr<Object>>& RootObject::get() const {
  return expressions_;
}

void RootObject::add(boost::local_shared_ptr<Object> expression) noexcept(false) {
  expressions_.push_back(std::move(expression));
}

}// namespace ast
