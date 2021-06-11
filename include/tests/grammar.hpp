#ifndef TEST_GRAMMAR_HPP
#define TEST_GRAMMAR_HPP

#include <unordered_map>

#include "../lexer/lexeme.hpp"

static inline const std::unordered_map<std::string, lexeme_t> test_keywords {
    {"for", lexeme_t::kw_for},
    {"while", lexeme_t::kw_while},
    {"if", lexeme_t::kw_if},
    {"else", lexeme_t::kw_else},
    {"return", lexeme_t::kw_return}
};

static inline const std::unordered_map<std::string, lexeme_t> test_operators {
    {"==", lexeme_t::equal},
    {"++", lexeme_t::inc},
    {"--", lexeme_t::dec},
    {"+", lexeme_t::plus},
    {"-", lexeme_t::minus},
    {"*", lexeme_t::star},
    {"/", lexeme_t::slash},
    {"=", lexeme_t::assign},
    {"+=", lexeme_t::plus_assign},
    {"*=", lexeme_t::star_assign},
    {"-=", lexeme_t::minus_assign},
    {"/=", lexeme_t::slash_assign},

    {".", lexeme_t::dot},
    {",", lexeme_t::comma},

    {"(", lexeme_t::left_paren},
    {")", lexeme_t::right_paren},
    {"{", lexeme_t::left_brace},
    {"}", lexeme_t::right_brace},

    {":", lexeme_t::colon},
    {";", lexeme_t::semicolon},
};

#endif //TEST_GRAMMAR_HPP
