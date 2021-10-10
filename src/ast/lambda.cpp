#include "../../include/ast/ast.hpp"

#include <utility>

namespace ast {

Lambda::Lambda(std::string name, std::vector<boost::local_shared_ptr<Object>> arguments, boost::local_shared_ptr<Block> body) noexcept(true)
  : name_(std::move(name))
  , arguments_(std::move(arguments))
  , body_(std::move(body)) {}

std::string Lambda::name() const noexcept(true) {
  return name_;
}

const std::vector<boost::local_shared_ptr<Object>>& Lambda::arguments() const noexcept(true) {
  return arguments_;
}

const boost::local_shared_ptr<Block>& Lambda::body() const noexcept(true) {
  return body_;
}

}// namespace ast
