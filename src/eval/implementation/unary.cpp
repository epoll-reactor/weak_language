#include "../../../include/eval/implementation/unary.hpp"

#include "../../../include/error/eval_error.hpp"

template <typename Number>
ALWAYS_INLINE static void compute_unary(token_t type, Number& unary) noexcept(false) {
  // clang-format off
  switch (type) {
    case token_t::INC: { ++unary; return; }
    case token_t::DEC: { --unary; return; }
    default: { throw EvalError("Unknown unary operator: {}", dispatch_token(type)); }
  }
  // clang-format on
}

void eval_context::unary_implementation(token_t unary_type, boost::local_shared_ptr<ast::Object>& expression, bool& failed) noexcept(false) {
  switch (expression->ast_type()) {
    case ast::type_t::INTEGER: {
      size_t& value = static_cast<ast::Integer*>(expression.get())->value();
      compute_unary(unary_type, value);
      return;
    }
    case ast::type_t::FLOAT: {
      double& value = static_cast<ast::Float*>(expression.get())->value();
      compute_unary(unary_type, value);
      return;
    }
    default: {
      failed = true;
    }
  }
}
