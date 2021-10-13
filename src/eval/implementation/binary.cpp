#include "../../../include/eval/implementation/binary.hpp"

#include "../../../include/ast/ast.hpp"
#include "../../../include/error/eval_error.hpp"

#include <any>
#include <boost/pool/pool_alloc.hpp>
#include <variant>

template <typename LeftOperand, typename RightOperand>
ALWAYS_INLINE static constexpr bool comparison_implementation(token_t type, LeftOperand l, RightOperand r) noexcept(false) {
  // clang-format off
  switch (type) {
    case token_t::eq: { return l == r; }
    case token_t::neq: { return l != r; }
    case token_t::ge: { return l >= r; }
    case token_t::gt: { return l > r; }
    case token_t::le: { return l <= r; }
    case token_t::lt: { return l < r; }
    default:
      throw EvalError("Incorrect binary expression: {}", dispatch_token(type));
  }
  // clang-format on
}

template <typename LeftFloatingPoint, typename RightFloatingPoint>
ALWAYS_INLINE static constexpr double floating_point_arithmetic_implementation(token_t type, LeftFloatingPoint l, RightFloatingPoint r) noexcept(false) {
  // clang-format off
  switch (type) {
    case token_t::plus: { return l + r; }
    case token_t::minus: { return l - r; }
    case token_t::star: { return l * r; }
    case token_t::slash: { return l / r; }
    default:
      return comparison_implementation<LeftFloatingPoint, RightFloatingPoint>(type, l, r);
  }
  // clang-format on
}

template <typename LeftIntegral, typename RightIntegral>
ALWAYS_INLINE static constexpr int32_t integral_arithmetic_implementation(token_t type, LeftIntegral l, RightIntegral r) noexcept(false) {
  // clang-format off
  switch (type) {
    case token_t::plus: { return l + r; }
    case token_t::minus: { return l - r; }
    case token_t::star: { return l * r; }
    case token_t::slash: { return l / r; }
    case token_t::mod: { return l % r; }
    case token_t::slli: { return l << r; }
    case token_t::srli: { return l >> r; }
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
    case token_t::plus_assign: { return token_t::plus; }
    case token_t::minus_assign: { return token_t::minus; }
    case token_t::star_assign: { return token_t::star; }
    case token_t::slash_assign: { return token_t::slash; }
    case token_t::xor_assign: { return token_t::bit_xor; }
    case token_t::or_assign: { return token_t::bit_or; }
    case token_t::and_assign: { return token_t::bit_and; }
    case token_t::srli_assign: { return token_t::srli;}
    case token_t::slli_assign: { return token_t::slli; }
    /// Never executes due to the `token_traits::is_assign_operator` checks
    default:
      return token_t::end_of_data;
  }
  // clang-format on
}

namespace {
template <typename T>
class allocator_holder {
  using allocator_t = boost::fast_pool_allocator<T>;

public:
  static allocator_holder<T>& create() {
    static allocator_holder<T> holder;
    return holder;
  }
  allocator_t& get_allocator() {
    return allocator_;
  }

private:
  allocator_t allocator_;
};

}// namespace

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
      /* Unreachable. */ return nullptr;
    }
  }
}
#undef MAKE_PAIR

boost::local_shared_ptr<ast::Object> eval_context::assign_binary_implementation(token_t type, const boost::local_shared_ptr<ast::Object>& lhs, const boost::local_shared_ptr<ast::Object>& rhs) noexcept(false) {
  if (lhs->ast_type() != rhs->ast_type()) {
    throw EvalError("Invalid binary operands");
  }
  const auto ast_type = lhs->ast_type();
  if (ast_type == ast::type_t::INTEGER) {
    size_t& l = static_cast<ast::Integer*>(lhs.get())->value();
    size_t& r = static_cast<ast::Integer*>(rhs.get())->value();
    l = integral_arithmetic_implementation(resolve_assign_operator(type), l, r);
  }
  if (ast_type == ast::type_t::FLOAT) {
    double& l = static_cast<ast::Float*>(lhs.get())->value();
    double& r = static_cast<ast::Float*>(rhs.get())->value();
    l = floating_point_arithmetic_implementation(resolve_assign_operator(type), l, r);
  }
  return lhs;
}