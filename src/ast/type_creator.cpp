#include "../../include/ast/ast.hpp"

namespace ast {

TypeCreator::TypeCreator(std::string name, std::vector<boost::local_shared_ptr<Object>> arguments) noexcept(true)
  : name_(std::move(name))
  , arguments_(std::move(arguments)) {}

const std::string& TypeCreator::name() const noexcept(true) {
  return name_;
}

const std::vector<boost::local_shared_ptr<Object>>& TypeCreator::arguments() const noexcept(true) {
  return arguments_;
}

}// namespace ast
