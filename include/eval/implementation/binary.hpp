#ifndef WEAK_EVAL_IMPLEMENTATION_BINARY_HPP
#define WEAK_EVAL_IMPLEMENTATION_BINARY_HPP

#include "../../ast/ast.hpp"
#include "../../lexer/token.hpp"

#include <boost/smart_ptr/local_shared_ptr.hpp>

namespace ast {
class Object;
}

namespace eval_context {
boost::local_shared_ptr<ast::Object> binary_implementation(
    ast::type_t left_type,
    ast::type_t right_type,
    token_t operation_type,
    const boost::local_shared_ptr<ast::Object>& lhs,
    const boost::local_shared_ptr<ast::Object>& rhs) noexcept(false);

/// @throws EvalError if operator is invalid
/// @throws EvalError if expression types are mismatch
boost::local_shared_ptr<ast::Object> assign_binary_implementation(
    token_t type,
    const boost::local_shared_ptr<ast::Object>& lhs,
    const boost::local_shared_ptr<ast::Object>& rhs) noexcept(false);

}// namespace eval_context

#endif// WEAK_EVAL_IMPLEMENTATION_BINARY_HPP
