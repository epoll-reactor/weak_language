#include <variant>

#include "../../../include/ast/ast.hpp"
#include "../../../include/common_defs.hpp"
#include "../../../include/error/eval_error.hpp"
#include "../../../include/eval/implementation/binary.hpp"

template <typename LeftOperand, typename RightOperand>
ALWAYS_INLINE static bool comparison_implementation(lexeme_t type, LeftOperand l, RightOperand r) noexcept(false)
{
    switch (type)
    {
        case lexeme_t::eq:      return l == r;
        case lexeme_t::neq:     return l != r;
        case lexeme_t::ge:      return l >= r;
        case lexeme_t::gt:      return l > r;
        case lexeme_t::le:      return l <= r;
        case lexeme_t::lt:      return l < r;
        default:
            throw EvalError("Incorrect binary expression");
    }
}

template <typename LeftFloatingPoint, typename RightFloatingPoint>
ALWAYS_INLINE static double floating_point_arithmetic_implementation(lexeme_t type, LeftFloatingPoint l, RightFloatingPoint r) noexcept(false)
{
    switch (type)
    {
        case lexeme_t::plus:    return l + r;
        case lexeme_t::minus:   return l - r;
        case lexeme_t::star:    return l * r;
        case lexeme_t::slash:   return l / r;
        default:
            return comparison_implementation<LeftFloatingPoint, RightFloatingPoint>(type, l, r);
    }
}

template <typename LeftIntegral, typename RightIntegral>
ALWAYS_INLINE static constexpr int32_t integral_arithmetic_implementation(lexeme_t type, LeftIntegral l, RightIntegral r) noexcept(false)
{
    switch (type)
    {
        case lexeme_t::plus:    return l + r;
        case lexeme_t::minus:   return l - r;
        case lexeme_t::star:    return l * r;
        case lexeme_t::slash:   return l / r;
        case lexeme_t::mod:     return l % r;
        case lexeme_t::slli:    return l << r;
        case lexeme_t::srli:    return l >> r;
        default:
            return comparison_implementation<LeftIntegral, RightIntegral>(type, l, r);
    }
}

template <typename LeftAST, typename RightAST>
ALWAYS_INLINE static constexpr std::variant<int32_t, double> arithmetic(lexeme_t type, const ast::Object* lhs, const ast::Object* rhs) noexcept(false)
{
    if constexpr (std::is_same_v<ast::Integer, LeftAST> && std::is_same_v<ast::Integer, RightAST>)
        return integral_arithmetic_implementation(
            type,
            static_cast<const ast::Integer*>(lhs)->value(),
            static_cast<const ast::Integer*>(rhs)->value()
        );

    return floating_point_arithmetic_implementation(
        type,
        static_cast<const LeftAST*>(lhs)->value(),
        static_cast<const RightAST*>(rhs)->value()
    );
}

boost::local_shared_ptr<ast::Object> internal::i_i_binary_implementation(lexeme_t binary_type, const boost::local_shared_ptr<ast::Object>& lhs, const boost::local_shared_ptr<ast::Object>& rhs) noexcept(false) {
    return boost::make_local_shared<ast::Integer>(std::get<int32_t>(arithmetic<ast::Integer, ast::Integer>(binary_type, lhs.get(), rhs.get())));
}
boost::local_shared_ptr<ast::Object> internal::i_f_binary_implementation(lexeme_t binary_type, const boost::local_shared_ptr<ast::Object>& lhs, const boost::local_shared_ptr<ast::Object>& rhs) noexcept(false) {
    return boost::make_local_shared<ast::Float>(std::get<double>(arithmetic<ast::Integer, ast::Float>(binary_type, lhs.get(), rhs.get())));
}
boost::local_shared_ptr<ast::Object> internal::f_i_binary_implementation(lexeme_t binary_type, const boost::local_shared_ptr<ast::Object>& lhs, const boost::local_shared_ptr<ast::Object>& rhs) noexcept(false) {
    return boost::make_local_shared<ast::Float>(std::get<double>(arithmetic<ast::Float, ast::Integer>(binary_type, lhs.get(), rhs.get())));
}
boost::local_shared_ptr<ast::Object> internal::f_f_binary_implementation(lexeme_t binary_type, const boost::local_shared_ptr<ast::Object>& lhs, const boost::local_shared_ptr<ast::Object>& rhs) noexcept(false) {
    return boost::make_local_shared<ast::Float>(std::get<double>(arithmetic<ast::Float, ast::Float>(binary_type, lhs.get(), rhs.get())));
}
