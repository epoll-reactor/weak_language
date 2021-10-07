#include "../../include/ast/ast.hpp"

namespace ast {

Float::Float(std::string_view data) noexcept(false)
  : data_(std::stod(data.data())) {}

Float::Float(double data) noexcept(true)
  : data_(data) {}

double& Float::value() noexcept(true) {
  return data_;
}

const double& Float::value() const noexcept(true) {
  return data_;
}

}// namespace ast
