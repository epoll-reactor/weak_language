#include "../../include/ast/ast.hpp"

namespace ast {

Symbol::Symbol(std::string name) noexcept(true)
  : name_(std::move(name)) {}

const std::string& Symbol::name() const noexcept(true) {
  return name_;
}

}// namespace ast
