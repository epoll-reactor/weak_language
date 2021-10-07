#include "../../include/ast/ast.hpp"

namespace ast {

Integer::Integer(std::string_view data) noexcept(false)
  : data_(std::stoi(data.data())) {}

Integer::Integer(size_t data) noexcept(true)
  : data_(data) {}

size_t& Integer::value() noexcept(true) {
  return data_;
}

const size_t& Integer::value() const noexcept(true) {
  return data_;
}

}// namespace ast
