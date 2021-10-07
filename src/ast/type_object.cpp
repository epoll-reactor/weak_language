#include "../../include/ast/ast.hpp"

namespace ast {

TypeObject::TypeObject(std::vector<std::pair<std::string, boost::local_shared_ptr<Object>>> arguments) noexcept(true)
  : arguments_(std::move(arguments)) {}

const std::vector<std::pair<std::string, boost::local_shared_ptr<Object>>>& TypeObject::fields() const noexcept(false) {
  return arguments_;
}

}// namespace ast