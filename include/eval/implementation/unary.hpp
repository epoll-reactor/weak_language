#ifndef WEAKLANGUAGE_UNARY_HPP
#define WEAKLANGUAGE_UNARY_HPP

#include "../../ast/ast.hpp"

namespace internal {

boost::intrusive_ptr<ast::Object> unary_implementation(
    ast::ast_type_t ast_type,
    lexeme_t unary_type,
    const boost::intrusive_ptr<ast::Object>&  expression
) noexcept(false);

} // namespace internal

#endif //WEAKLANGUAGE_UNARY_HPP
