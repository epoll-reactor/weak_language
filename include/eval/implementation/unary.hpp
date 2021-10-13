#ifndef WEAK_EVAL_IMPLEMENTATION_UNARY_HPP
#define WEAK_EVAL_IMPLEMENTATION_UNARY_HPP

#include "../../ast/ast.hpp"

namespace eval_context {

/// @throws EvalError if operator is invalid
void unary_implementation(
    ast::type_t ast_type,
    token_t unary_type,
    boost::local_shared_ptr<ast::Object>& expression,
    bool& failed) noexcept(false);

}// namespace eval_context

#endif//WEAK_EVAL_IMPLEMENTATION_UNARY_HPP
