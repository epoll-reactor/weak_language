#include "../../../include/common_defs.hpp"
#include "../../../include/error/eval_error.hpp"
#include "../../../include/eval/implementation/unary.hpp"

ALWAYS_INLINE static void integral_unary_implementation(lexeme_t type, int32_t& unary) noexcept(false)
{
    if (  LIKELY(type == lexeme_t::inc)) { ++unary; return; }
    if (UNLIKELY(type == lexeme_t::dec)) { --unary; return; }
    throw EvalError("Unknown unary operator");
}

ALWAYS_INLINE static void floating_point_unary_implementation(lexeme_t type, double& unary) noexcept(false)
{
    if (  LIKELY(type == lexeme_t::inc)) { ++unary; return; }
    if (UNLIKELY(type == lexeme_t::dec)) { --unary; return; }
    throw EvalError("Unknown unary operator");
}

boost::local_shared_ptr<ast::Object> internal::unary_implementation(ast::ast_type_t ast_type, lexeme_t unary_type, const boost::local_shared_ptr<ast::Object>& expression)  noexcept(false)
{
    if (ast_type == ast::ast_type_t::INTEGER)
    {
        int& value = static_cast<ast::Integer*>(expression.get())->value();
        integral_unary_implementation(unary_type, value);
        return expression;
    }

    if (ast_type == ast::ast_type_t::FLOAT)
    {
        double& value = static_cast<ast::Float*>(expression.get())->value();
        floating_point_unary_implementation(unary_type, value);
        return expression;
    }

    return nullptr;
}
