#include "../../include/ast/ast.hpp"

namespace ast {

String::String(std::string data) noexcept(true)
  : data_(std::move(data)) {}

const std::string& String::value() const noexcept(true) {
  return data_;
}

}// namespace ast
