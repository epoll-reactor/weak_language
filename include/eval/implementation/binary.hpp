#ifndef WEAKLANGUAGE_BINARY_HPP
#define WEAKLANGUAGE_BINARY_HPP

#include "../../lexer/lexeme.hpp"

namespace ast { class Object; }

namespace internal {

ast::Object* i_i_binary_implementation(
    lexeme_t binary_type,
    const ast::Object* lhs,
    const ast::Object* rhs
) noexcept(false);

ast::Object* i_f_binary_implementation(
    lexeme_t binary_type,
    const ast::Object* lhs,
    const ast::Object* rhs
) noexcept(false);

ast::Object* f_i_binary_implementation(
    lexeme_t binary_type,
    const ast::Object* lhs,
    const ast::Object* rhs
) noexcept(false);

ast::Object* f_f_binary_implementation(
    lexeme_t binary_type,
    const ast::Object* lhs,
    const ast::Object* rhs
) noexcept(false);

} // namespace internal

#endif //WEAKLANGUAGE_BINARY_HPP