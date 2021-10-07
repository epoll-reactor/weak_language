#include "../../include/ast/ast.hpp"

#include <utility>

namespace ast {

While::While(boost::local_shared_ptr<Object> exit_condition, boost::local_shared_ptr<Block> block) noexcept(true)
  : exit_condition_(std::move(exit_condition))
  , block_(std::move(block)) {}

const boost::local_shared_ptr<Object>& While::exit_condition() const noexcept(true) {
  return exit_condition_;
}

const boost::local_shared_ptr<Block>& While::body() const noexcept(true) {
  return block_;
}

}// namespace ast
