#include "../../../include/eval/implementation/unary.hpp"

#include "../../../include/error/eval_error.hpp"

// clang-format off
ALWAYS_INLINE static void integral_unary_implementation(token_t type, size_t& unary) noexcept(false) {
  if (  LIKELY(type == token_t::inc)) { ++unary; return; }
  if (UNLIKELY(type == token_t::dec)) { --unary; return; }
  throw EvalError("Unknown unary operator: {}", dispatch_token(type));
}

ALWAYS_INLINE static void floating_point_unary_implementation(token_t type, double& unary) noexcept(false) {
  if (  LIKELY(type == token_t::inc)) { ++unary; return; }
  if (UNLIKELY(type == token_t::dec)) { --unary; return; }
  throw EvalError("Unknown unary operator: {}", dispatch_token(type));
}
// clang-format on

boost::local_shared_ptr<ast::Object> eval_context::unary_implementation(ast::type_t ast_type, token_t unary_type, const boost::local_shared_ptr<ast::Object>& expression) noexcept(false) {
  switch (ast_type) {
    case ast::type_t::INTEGER: {
      size_t& value = static_cast<ast::Integer*>(expression.get())->value();
      integral_unary_implementation(unary_type, value);
      return expression;
    }
    case ast::type_t::FLOAT: {
      double& value = static_cast<ast::Float*>(expression.get())->value();
      floating_point_unary_implementation(unary_type, value);
      return expression;
    }
    default:
      return nullptr;
  }
}
