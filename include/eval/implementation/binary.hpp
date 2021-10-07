#ifndef WEAK_EVAL_IMPLEMENTATION_BINARY_HPP
#define WEAK_EVAL_IMPLEMENTATION_BINARY_HPP

#include "../../lexer/token.hpp"

#include <boost/smart_ptr/local_shared_ptr.hpp>

namespace ast {
class Object;
}

namespace internal {

/// @throws EvalError if operator is invalid
boost::local_shared_ptr<ast::Object> i_i_binary_implementation(
    token_t type,
    const boost::local_shared_ptr<ast::Object>& lhs,
    const boost::local_shared_ptr<ast::Object>& rhs) noexcept(false);

/// @throws EvalError if operator is invalid
boost::local_shared_ptr<ast::Object> i_f_binary_implementation(
    token_t type,
    const boost::local_shared_ptr<ast::Object>& lhs,
    const boost::local_shared_ptr<ast::Object>& rhs) noexcept(false);

/// @throws EvalError if operator is invalid
boost::local_shared_ptr<ast::Object> f_i_binary_implementation(
    token_t type,
    const boost::local_shared_ptr<ast::Object>& lhs,
    const boost::local_shared_ptr<ast::Object>& rhs) noexcept(false);

/// @throws EvalError if operator is invalid
boost::local_shared_ptr<ast::Object> f_f_binary_implementation(
    token_t type,
    const boost::local_shared_ptr<ast::Object>& lhs,
    const boost::local_shared_ptr<ast::Object>& rhs) noexcept(false);

/// @throws EvalError if operator is invalid
/// @throws EvalError if expression types are mismatch
boost::local_shared_ptr<ast::Object> assign_binary_implementation(
    token_t type,
    const boost::local_shared_ptr<ast::Object>& lhs,
    const boost::local_shared_ptr<ast::Object>& rhs) noexcept(false);

}// namespace internal

#endif// WEAK_EVAL_IMPLEMENTATION_BINARY_HPP
