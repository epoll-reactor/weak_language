#include "../../include/ast/ast.hpp"

namespace ast {

LambdaCall::LambdaCall(std::string name, std::vector<boost::local_shared_ptr<Object>> arguments) noexcept(true)
  : name_(std::move(name))
  , arguments_(std::move(arguments)) {}

const std::string& LambdaCall::name() const noexcept(true) {
  return name_;
}

const std::vector<boost::local_shared_ptr<Object>>& LambdaCall::arguments() const noexcept(true) {
  return arguments_;
}

}// namespace ast
