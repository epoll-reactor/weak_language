#include "../../include/ast/ast.hpp"

namespace ast {

Array::Array(std::vector<boost::local_shared_ptr<Object>> elements) noexcept(true)
  : elements_(std::move(elements)) {}

std::vector<boost::local_shared_ptr<Object>>& Array::elements() noexcept(true) {
  return elements_;
}

}// namespace ast
