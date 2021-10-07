#include "../../include/ast/ast.hpp"

namespace ast {

FunctionCall::FunctionCall(std::string name, std::vector<boost::local_shared_ptr<Object>> arguments) noexcept(true)
  : name_(std::move(name))
  , arguments_(std::move(arguments)) {}

const std::string& FunctionCall::name() const noexcept(true) {
  return name_;
}

const std::vector<boost::local_shared_ptr<Object>>& FunctionCall::arguments() const noexcept(true) {
  return arguments_;
}

}// namespace ast
