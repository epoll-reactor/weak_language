#include "../../include/ast/ast.hpp"

namespace ast {

void For::set_init(boost::local_shared_ptr<Object> init) noexcept(true) {
  init_ = std::move(init);
}

void For::set_exit_condition(boost::local_shared_ptr<Object> exit_condition) noexcept(true) {
  exit_condition_ = std::move(exit_condition);
}

void For::set_increment(boost::local_shared_ptr<Object> increment) noexcept(true) {
  increment_ = std::move(increment);
}

void For::set_body(boost::local_shared_ptr<Block> block) noexcept(true) {
  block_ = std::move(block);
}

const boost::local_shared_ptr<Object>& For::loop_init() const noexcept(true) {
  return init_;
}

const boost::local_shared_ptr<Object>& For::exit_condition() const noexcept(true) {
  return exit_condition_;
}

const boost::local_shared_ptr<Object>& For::increment() const noexcept(true) {
  return increment_;
}

const boost::local_shared_ptr<Block>& For::body() const noexcept(true) {
  return block_;
}

}// namespace ast
