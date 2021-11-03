#include "../../include/ast/ast.hpp"

namespace ast {

TypeDefinition::TypeDefinition(std::string name, std::vector<std::string> fields) noexcept(true)
  : name_(std::move(name))
  , fields_(std::move(fields)) {}

const std::string& TypeDefinition::name() const noexcept(true) {
  return name_;
}

const std::vector<std::string>& TypeDefinition::fields() const noexcept(true) {
  return fields_;
}

}// namespace ast
