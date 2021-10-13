#include "../../../include/eval/implementation/unary.hpp"

#include "../../../include/error/eval_error.hpp"

template <typename Number>
ALWAYS_INLINE static void compute_unary(token_t type, Number& unary) noexcept(false) {
  // clang-format off
  switch (type) {
    case token_t::inc: { ++unary; return; }
    case token_t::dec: { --unary; return; }
    default: { throw EvalError("Unknown unary operator: {}", dispatch_token(type)); }
  }
  // clang-format on
}

void eval_context::unary_implementation(ast::type_t ast_type, token_t unary_type, boost::local_shared_ptr<ast::Object>& expression, bool& failed) noexcept(false) {
  switch (ast_type) {
    case ast::type_t::INTEGER: {
      size_t& value = static_cast<ast::Integer*>(expression.get())->value();
      compute_unary(unary_type, value);
      break;
    }
    case ast::type_t::FLOAT: {
      double& value = static_cast<ast::Float*>(expression.get())->value();
      compute_unary(unary_type, value);
      break;
    }
    default: {
      failed = true;
    }
  }
}
