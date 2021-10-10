#include "../../include/ast/ast.hpp"

namespace ast {

TypeDefinition::TypeDefinition(std::string_view name, std::vector<std::string> fields) noexcept(true)
  : type_name_(name)
  , names_(std::move(fields)) {}

const std::string& TypeDefinition::type_name() const noexcept(true) {
  return type_name_;
}

const std::vector<std::string>& TypeDefinition::names() const noexcept(true) {
  return names_;
}

}// namespace ast
