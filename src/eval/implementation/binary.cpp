#include "../../../include/eval/implementation/binary.hpp"

#include "../../../include/error/eval_error.hpp"

#include <any>
#include <boost/pool/pool_alloc.hpp>
#include <variant>

template <typename LeftOperand, typename RightOperand>
ALWAYS_INLINE static constexpr bool comparison_implementation(token_t type, LeftOperand l, RightOperand r) noexcept(false) {
  // clang-format off
  switch (type) {
    case token_t::EQ: { return l == r; }
    case token_t::NEQ: { return l != r; }
    case token_t::GE: { return l >= r; }
    case token_t::GT: { return l > r; }
    case token_t::LE: { return l <= r; }
    case token_t::LT: { return l < r; }
    default:
      throw EvalError("Incorrect binary expression: {}", dispatch_token(type));
  }
  // clang-format on
}

template <typename LeftFloatingPoint, typename RightFloatingPoint>
ALWAYS_INLINE static constexpr double floating_point_arithmetic_implementation(token_t type, LeftFloatingPoint l, RightFloatingPoint r) noexcept(false) {
  // clang-format off
  switch (type) {
    case token_t::PLUS: { return l + r; }
    case token_t::MINUS: { return l - r; }
    case token_t::STAR: { return l * r; }
    case token_t::SLASH: { return l / r; }
    default:
      return comparison_implementation<LeftFloatingPoint, RightFloatingPoint>(type, l, r);
  }
  // clang-format on
}

template <typename LeftIntegral, typename RightIntegral>
ALWAYS_INLINE static constexpr int32_t integral_arithmetic_implementation(token_t type, LeftIntegral l, RightIntegral r) noexcept(false) {
  // clang-format off
  switch (type) {
    case token_t::PLUS: { return l + r; }
    case token_t::MINUS: { return l - r; }
    case token_t::STAR: { return l * r; }
    case token_t::SLASH: { return l / r; }
    case token_t::MOD: { return l % r; }
    case token_t::SLLI: { return l << r; }
    case token_t::SRLI: { return l >> r; }
    default:
      return comparison_implementation<LeftIntegral, RightIntegral>(type, l, r);
  }
  // clang-format on
}

template <typename LeftAST, typename RightAST>
ALWAYS_INLINE static constexpr std::variant<int32_t, double> arithmetic(token_t type, const ast::Object* lhs, const ast::Object* rhs) noexcept(false) {
  if constexpr (std::is_same_v<ast::Integer, LeftAST> && std::is_same_v<ast::Integer, RightAST>) {
    return integral_arithmetic_implementation(
        type,
        static_cast<const ast::Integer*>(lhs)->value(),
        static_cast<const ast::Integer*>(rhs)->value());
  }
  return floating_point_arithmetic_implementation(
      type,
      static_cast<const LeftAST*>(lhs)->value(),
      static_cast<const RightAST*>(rhs)->value());
}

ALWAYS_INLINE static constexpr token_t resolve_assign_operator(token_t tok) noexcept(true) {
  // clang-format off
  switch (tok) {
    case token_t::PLUS_ASSIGN: { return token_t::PLUS; }
    case token_t::MINUS_ASSIGN: { return token_t::MINUS; }
    case token_t::STAR_ASSIGN: { return token_t::STAR; }
    case token_t::SLASH_ASSIGN: { return token_t::SLASH; }
    case token_t::XOR_ASSIGN: { return token_t::BIT_XOR; }
    case token_t::OR_ASSIGN: { return token_t::BIT_OR; }
    case token_t::AND_ASSIGN: { return token_t::BIT_AND; }
    case token_t::SRLI_ASSIGN: { return token_t::SRLI;}
    case token_t::SLLI_ASSIGN: { return token_t::SLLI; }
    /// Never executes due to the `token_traits::is_assign_operator` checks
    default:
      return token_t::END_OF_DATA;
  }
  // clang-format on
}

template <typename Result, typename LeftAST, typename RightAST>
ALWAYS_INLINE auto create_binary(
    token_t operation,
    const boost::local_shared_ptr<ast::Object>& lhs,
    const boost::local_shared_ptr<ast::Object>& rhs) noexcept(false) {
  if constexpr (std::is_same_v<Result, ast::Integer>) {
    return boost::make_local_shared<Result>(std::get<int32_t>(arithmetic<LeftAST, RightAST>(operation, lhs.get(), rhs.get())));
  } else if constexpr (std::is_same_v<Result, ast::Float>) {
    return boost::make_local_shared<Result>(std::get<double>(arithmetic<LeftAST, RightAST>(operation, lhs.get(), rhs.get())));
  }
}

#define ENUM_PAIR(x, y) ((static_cast<uint32_t>(x)) | ((static_cast<uint32_t>(y)) << 16))

boost::local_shared_ptr<ast::Object> eval_context::binary_implementation(ast::type_t left_type, ast::type_t right_type, token_t operation, const boost::local_shared_ptr<ast::Object>& lhs, const boost::local_shared_ptr<ast::Object>& rhs) noexcept(false) {
  switch (ENUM_PAIR(left_type, right_type)) {
    case ENUM_PAIR(ast::type_t::INTEGER, ast::type_t::INTEGER): {
      return create_binary<ast::Integer, ast::Integer, ast::Integer>(operation, lhs, rhs);
    }
    case ENUM_PAIR(ast::type_t::INTEGER, ast::type_t::FLOAT): {
      return create_binary<ast::Float, ast::Integer, ast::Float>(operation, lhs, rhs);
    }
    case ENUM_PAIR(ast::type_t::FLOAT, ast::type_t::FLOAT): {
      return create_binary<ast::Float, ast::Float, ast::Float>(operation, lhs, rhs);
    }
    case ENUM_PAIR(ast::type_t::FLOAT, ast::type_t::INTEGER): {
      return create_binary<ast::Float, ast::Float, ast::Integer>(operation, lhs, rhs);
    }
    default: {
      throw EvalError("wrong binary types");
    }
  }
}

#undef MAKE_PAIR

boost::local_shared_ptr<ast::Object> eval_context::assign_binary_implementation(token_t type, const boost::local_shared_ptr<ast::Object>& lhs, const boost::local_shared_ptr<ast::Object>& rhs) noexcept(false) {
  if (lhs->ast_type() != rhs->ast_type()) {
    throw EvalError("Invalid binary operands");
  }
  switch (lhs->ast_type()) {
    case ast::type_t::INTEGER: {
      size_t& l = static_cast<ast::Integer*>(lhs.get())->value();
      size_t& r = static_cast<ast::Integer*>(rhs.get())->value();
      l = integral_arithmetic_implementation(resolve_assign_operator(type), l, r);
      return lhs;
    }
    case ast::type_t::FLOAT: {
      double& l = static_cast<ast::Float*>(lhs.get())->value();
      double& r = static_cast<ast::Float*>(rhs.get())->value();
      l = floating_point_arithmetic_implementation(resolve_assign_operator(type), l, r);
      return lhs;
    }
    default: {
      throw EvalError("Invalid binary operands");
    }
  }
}