#include "../../include/ast/ast.hpp"

namespace ast {

ArraySubscriptOperator::ArraySubscriptOperator(std::string name, boost::local_shared_ptr<Object> index) noexcept(true)
  : name_(std::move(name))
  , index_(std::move(index)) {}

const std::string& ArraySubscriptOperator::symbol_name() const noexcept(true) {
  return name_;
}

const boost::local_shared_ptr<Object>& ArraySubscriptOperator::index() const noexcept(true) {
  return index_;
}

}// namespace ast
