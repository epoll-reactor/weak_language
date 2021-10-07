#include "../../include/ast/ast.hpp"

namespace ast {

If::If(boost::local_shared_ptr<Object> exit_condition, boost::local_shared_ptr<Block> body) noexcept(true)
  : exit_condition_(std::move(exit_condition))
  , body_(std::move(body)) {}

If::If(boost::local_shared_ptr<Object> exit_condition, boost::local_shared_ptr<Block> body, boost::local_shared_ptr<Block> else_body) noexcept(true)
  : exit_condition_(std::move(exit_condition))
  , body_(std::move(body))
  , else_body_(std::move(else_body)) {}

const boost::local_shared_ptr<Object>& If::condition() const noexcept(true) {
  return exit_condition_;
}

const boost::local_shared_ptr<Block>& If::body() const noexcept(true) {
  return body_;
}

const boost::local_shared_ptr<Block>& If::else_body() const noexcept(true) {
  return else_body_;
}

}// namespace ast
