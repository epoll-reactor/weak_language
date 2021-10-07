#include "../../include/ast/ast.hpp"

namespace ast {

TypeFieldOperator::TypeFieldOperator(std::string type_name, std::string type_field) noexcept(true)
  : type_name_(std::move(type_name))
  , type_field_(std::move(type_field)) {}

const std::string& TypeFieldOperator::name() const noexcept(true) {
  return type_name_;
}

const std::string& TypeFieldOperator::field() const noexcept(true) {
  return type_field_;
}

}// namespace ast