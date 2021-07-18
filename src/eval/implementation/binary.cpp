#include "../../../include/eval/implementation/binary.hpp"

#include "../../../include/ast/ast.hpp"
#include "../../../include/error/eval_error.hpp"

#include <variant>

template <typename LeftOperand, typename RightOperand>
ALWAYS_INLINE static bool comparison_implementation(token_t type, LeftOperand l, RightOperand r) noexcept(false)
{
    switch (type) {
        case token_t::eq:      return l == r;
        case token_t::neq:     return l != r;
        case token_t::ge:      return l >= r;
        case token_t::gt:      return l > r;
        case token_t::le:      return l <= r;
        case token_t::lt:      return l < r;
        default:
            throw EvalError("Incorrect binary expression: {}", dispatch_token(type));
    }
}

template <typename LeftFloatingPoint, typename RightFloatingPoint>
ALWAYS_INLINE static double floating_point_arithmetic_implementation(token_t type, LeftFloatingPoint l, RightFloatingPoint r) noexcept(false)
{
    switch (type) {
        case token_t::plus:    return l + r;
        case token_t::minus:   return l - r;
        case token_t::star:    return l * r;
        case token_t::slash:   return l / r;
        default:
            return comparison_implementation<LeftFloatingPoint, RightFloatingPoint>(type, l, r);
    }
}

template <typename LeftIntegral, typename RightIntegral>
ALWAYS_INLINE static constexpr int32_t integral_arithmetic_implementation(token_t type, LeftIntegral l, RightIntegral r) noexcept(false)
{
    switch (type) {
        case token_t::plus:    return l + r;
        case token_t::minus:   return l - r;
        case token_t::star:    return l * r;
        case token_t::slash:   return l / r;
        case token_t::mod:     return l % r;
        case token_t::slli:    return l << r;
        case token_t::srli:    return l >> r;
        default:
            return comparison_implementation<LeftIntegral, RightIntegral>(type, l, r);
    }
}

template <typename LeftAST, typename RightAST>
ALWAYS_INLINE static constexpr std::variant<int32_t, double> arithmetic(token_t type, const ast::Object* lhs, const ast::Object* rhs) noexcept(false)
{
    if constexpr (std::is_same_v<ast::Integer, LeftAST> && std::is_same_v<ast::Integer, RightAST>) {
        return integral_arithmetic_implementation(
            type,
            static_cast<const ast::Integer*>(lhs)->value(),
            static_cast<const ast::Integer*>(rhs)->value()
        );
    }

    return floating_point_arithmetic_implementation(
        type,
        static_cast<const LeftAST*>(lhs)->value(),
        static_cast<const RightAST*>(rhs)->value()
    );
}

static constexpr token_t resolve_assign_operator(token_t token) noexcept(true)
{
    switch (token) {
        case token_t::plus_assign: return token_t::plus;
        case token_t::minus_assign: return token_t::minus;
        case token_t::star_assign: return token_t::star;
        case token_t::slash_assign: return token_t::slash;
        case token_t::xor_assign: return token_t::bit_xor;
        case token_t::or_assign: return token_t::bit_or;
        case token_t::and_assign: return token_t::bit_and;
        case token_t::srli_assign: return token_t::srli;
        case token_t::slli_assign: return token_t::slli;
        /// Never executes due to the `token_traits::is_assign_operator` checks
        default: return token_t::end_of_data;
    }
}

boost::local_shared_ptr<ast::Object> internal::i_i_binary_implementation(token_t binary_type, const boost::local_shared_ptr<ast::Object>& lhs, const boost::local_shared_ptr<ast::Object>& rhs) noexcept(false) {
    return boost::make_local_shared<ast::Integer>(std::get<int32_t>(arithmetic<ast::Integer, ast::Integer>(binary_type, lhs.get(), rhs.get())));
}
boost::local_shared_ptr<ast::Object> internal::i_f_binary_implementation(token_t binary_type, const boost::local_shared_ptr<ast::Object>& lhs, const boost::local_shared_ptr<ast::Object>& rhs) noexcept(false) {
    return boost::make_local_shared<ast::Float>(std::get<double>(arithmetic<ast::Integer, ast::Float>(binary_type, lhs.get(), rhs.get())));
}
boost::local_shared_ptr<ast::Object> internal::f_i_binary_implementation(token_t binary_type, const boost::local_shared_ptr<ast::Object>& lhs, const boost::local_shared_ptr<ast::Object>& rhs) noexcept(false) {
    return boost::make_local_shared<ast::Float>(std::get<double>(arithmetic<ast::Float, ast::Integer>(binary_type, lhs.get(), rhs.get())));
}
boost::local_shared_ptr<ast::Object> internal::f_f_binary_implementation(token_t binary_type, const boost::local_shared_ptr<ast::Object>& lhs, const boost::local_shared_ptr<ast::Object>& rhs) noexcept(false) {
    return boost::make_local_shared<ast::Float>(std::get<double>(arithmetic<ast::Float, ast::Float>(binary_type, lhs.get(), rhs.get())));
}

boost::local_shared_ptr<ast::Object> internal::assign_binary_implementation(token_t binary_type, const boost::local_shared_ptr<ast::Object>& lhs, const boost::local_shared_ptr<ast::Object>& rhs) noexcept(false)
{
    if (lhs->ast_type() != rhs->ast_type()) { throw EvalError("Invalid binary operands"); }

    const auto ast_type = lhs->ast_type();

    if (ast_type == ast::ast_type_t::INTEGER) {
        int& lhs_value = static_cast<ast::Integer*>(lhs.get())->value();
        int& rhs_value = static_cast<ast::Integer*>(rhs.get())->value();
        lhs_value = integral_arithmetic_implementation(resolve_assign_operator(binary_type), lhs_value, rhs_value);
    }

    if (ast_type == ast::ast_type_t::FLOAT) {
        double& lhs_value = static_cast<ast::Float*>(lhs.get())->value();
        double& rhs_value = static_cast<ast::Float*>(rhs.get())->value();
        lhs_value = floating_point_arithmetic_implementation(resolve_assign_operator(binary_type), lhs_value, rhs_value);
    }

    return lhs;
}