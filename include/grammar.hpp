#ifndef WEAK_GRAMMAR_HPP
#define WEAK_GRAMMAR_HPP

#include "lexer/token.hpp"

#include <unordered_map>

static inline const std::unordered_map<std::string, token_t> test_keywords{
    {"for", token_t::FOR},
    {"while", token_t::WHILE},
    {"if", token_t::IF},
    {"else", token_t::ELSE},
    {"return", token_t::RETURN},
    {"lambda", token_t::LAMBDA},
    {"load", token_t::LOAD},
    {"define-type", token_t::DEFINE_TYPE},
    {"new", token_t::NEW}};

static inline const std::unordered_map<std::string, token_t> test_operators{
    {"=", token_t::ASSIGN},

    {"!", token_t::NEGATION},

    {"==", token_t::EQ},
    {"!=", token_t::NEQ},
    {"<", token_t::LT},
    {"<=", token_t::LE},
    {">", token_t::GT},
    {">=", token_t::GE},

    {"++", token_t::INC},
    {"--", token_t::DEC},

    {"<<", token_t::SLLI},
    {">>", token_t::SRLI},
    {"&", token_t::BIT_AND},
    {"|", token_t::BIT_OR},
    {"^", token_t::BIT_XOR},
    {"&&", token_t::LOGICAL_AND},
    {"||", token_t::LOGICAL_OR},

    {"+", token_t::PLUS},
    {"-", token_t::MINUS},
    {"*", token_t::STAR},
    {"/", token_t::SLASH},
    {"%", token_t::MOD},

    {"+=", token_t::PLUS_ASSIGN},
    {"*=", token_t::STAR_ASSIGN},
    {"-=", token_t::MINUS_ASSIGN},
    {"/=", token_t::SLASH_ASSIGN},
    {"<<=", token_t::SLLI_ASSIGN},
    {">>=", token_t::SRLI_ASSIGN},
    {"&=", token_t::AND_ASSIGN},
    {"|=", token_t::OR_ASSIGN},
    {"^=", token_t::XOR_ASSIGN},

    {".", token_t::DOT},
    {",", token_t::COMMA},
    {":", token_t::COLON},
    {";", token_t::SEMICOLON},

    {"(", token_t::LEFT_PAREN},
    {")", token_t::RIGHT_PAREN},
    {"{", token_t::LEFT_BRACE},
    {"}", token_t::RIGHT_BRACE},
    {"[", token_t::LEFT_BOX_BRACE},
    {"]", token_t::RIGHT_BOX_BRACE},
};

#endif// WEAK_GRAMMAR_HPP
