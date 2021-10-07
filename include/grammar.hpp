#ifndef WEAK_GRAMMAR_HPP
#define WEAK_GRAMMAR_HPP

#include "lexer/token.hpp"

#include <unordered_map>

static inline const std::unordered_map<std::string, token_t> test_keywords{
    {"for", token_t::kw_for},
    {"while", token_t::kw_while},
    {"if", token_t::kw_if},
    {"else", token_t::kw_else},
    {"return", token_t::kw_return},
    {"fun", token_t::kw_function_decl},
    {"load", token_t::kw_load},
    {"define-type", token_t::kw_define_type},
    {"new", token_t::kw_new}};

static inline const std::unordered_map<std::string, token_t> test_operators{
    {"=", token_t::assign},

    {"!", token_t::negation},

    {"==", token_t::eq},
    {"!=", token_t::neq},
    {"<", token_t::lt},
    {"<=", token_t::le},
    {">", token_t::gt},
    {">=", token_t::ge},

    {"++", token_t::inc},
    {"--", token_t::dec},

    {"<<", token_t::slli},
    {">>", token_t::srli},
    {"&", token_t::bit_and},
    {"|", token_t::bit_or},
    {"^", token_t::bit_xor},
    {"&&", token_t::logical_and},
    {"||", token_t::logical_or},

    {"+", token_t::plus},
    {"-", token_t::minus},
    {"*", token_t::star},
    {"/", token_t::slash},
    {"%", token_t::mod},

    {"+=", token_t::plus_assign},
    {"*=", token_t::star_assign},
    {"-=", token_t::minus_assign},
    {"/=", token_t::slash_assign},
    {"<<=", token_t::slli_assign},
    {">>=", token_t::srli_assign},
    {"&=", token_t::and_assign},
    {"|=", token_t::or_assign},
    {"^=", token_t::xor_assign},

    {".", token_t::dot},
    {",", token_t::comma},
    {":", token_t::colon},
    {";", token_t::semicolon},

    {"(", token_t::left_paren},
    {")", token_t::right_paren},
    {"{", token_t::left_brace},
    {"}", token_t::right_brace},
    {"[", token_t::left_box_brace},
    {"]", token_t::right_box_brace},
};

#endif// WEAK_GRAMMAR_HPP
