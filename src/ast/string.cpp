#include "../../include/ast/ast.hpp"

namespace ast {

String::String(std::string data) noexcept(true)
  : data_(std::move(data)) {}

std::string& String::value() noexcept(true) {
  return data_;
}

const std::string& String::value() const noexcept(true) {
  return data_;
}

}// namespace ast
