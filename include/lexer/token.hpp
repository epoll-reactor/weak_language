#ifndef WEAK_LEXER_TOKEN_HPP
#define WEAK_LEXER_TOKEN_HPP

#include "../common_defs.hpp"

#include <string>

enum struct token_t {
  DOT,// .
  COMMA,// ,
  NEGATION,// !

  GT,// >
  GE,// >=
  LT,// <
  LE,// <=
  EQ,// ==
  NEQ,// !=

  MOD,// %
  PLUS,// +
  MINUS,// -
  INC,// ++
  DEC,// --
  STAR,// *
  SLASH,// /
  ASSIGN,// =

  SLLI,// <<
  SRLI,// >>

  BIT_AND,// &
  BIT_OR,// |
  BIT_XOR,// ^

  LOGICAL_AND,// &&
  LOGICAL_OR,// ||

  PLUS_ASSIGN,// +=
  MINUS_ASSIGN,// -=
  STAR_ASSIGN,// *=
  SLASH_ASSIGN,// /=
  SLLI_ASSIGN,// <<=
  SRLI_ASSIGN,// >>=
  AND_ASSIGN,// &=
  OR_ASSIGN,// |=
  XOR_ASSIGN,// ^=

  LEFT_PAREN,// (
  RIGHT_PAREN,// )
  LEFT_BRACE,// {
  RIGHT_BRACE,// }
  LEFT_BOX_BRACE,// [
  RIGHT_BOX_BRACE,// ]

  COLON,// :
  SEMICOLON,// ;

  FOR,
  WHILE,
  IF,
  ELSE,
  RETURN,
  LAMBDA,
  DEFINE_TYPE,
  NEW,
  LOAD,

  NUM,
  FLOAT,
  SYMBOL,
  STRING_LITERAL,
  NONE,
  END_OF_DATA
};

inline std::string dispatch_token(token_t tok) noexcept(true) {
  // clang-format off
  switch (tok) {
    case token_t::DOT: { return "."; }
    case token_t::COMMA: { return ","; }
    case token_t::PLUS: { return "+"; }
    case token_t::MINUS: { return "-"; }
    case token_t::INC: { return "++"; }
    case token_t::DEC: { return "--"; }
    case token_t::STAR: { return "*"; }
    case token_t::SLASH: { return "/"; }
    case token_t::ASSIGN: { return "="; }
    case token_t::PLUS_ASSIGN: { return "+="; }
    case token_t::MINUS_ASSIGN: { return "-="; }
    case token_t::STAR_ASSIGN: { return "*="; }
    case token_t::SLASH_ASSIGN: { return "/="; }
    case token_t::MOD: { return "%"; }
    case token_t::LEFT_PAREN: { return "("; }
    case token_t::RIGHT_PAREN: { return ")"; }
    case token_t::LEFT_BRACE: { return "{"; }
    case token_t::RIGHT_BRACE: { return "}"; }
    case token_t::LEFT_BOX_BRACE: { return "["; }
    case token_t::RIGHT_BOX_BRACE: { return "]"; }
    case token_t::COLON: { return ":"; }
    case token_t::SEMICOLON: { return ";"; }
    case token_t::EQ: { return "=="; }
    case token_t::GT: { return ">"; }
    case token_t::GE: { return ">="; }
    case token_t::LT: { return "<"; }
    case token_t::LE: { return "<="; }
    case token_t::NEGATION: { return "!"; }
    case token_t::NEQ: { return "!="; }
    case token_t::SRLI: { return ">>"; }
    case token_t::SLLI: { return "<<"; }
    case token_t::FLOAT: { return "<float>"; }
    case token_t::NUM: { return "<number>"; }
    case token_t::SYMBOL: { return "<symbol>"; }
    case token_t::STRING_LITERAL: { return "<string literal>"; }
    case token_t::NONE: { return "<none>"; }
    case token_t::END_OF_DATA: { return "<EOF>"; }
    case token_t::LAMBDA: { return "<lambda>"; }
    case token_t::FOR: { return "<for>"; }
    case token_t::WHILE: { return "<while>"; }
    case token_t::IF: { return "<if>"; }
    case token_t::LOAD: { return "<load>"; }
    case token_t::NEW: { return "<new>"; }
    default: { return "<unknown>"; }
  }
  // clang-format on
}

namespace token_traits {

ALWAYS_INLINE constexpr bool is_assign_operator(token_t tok) noexcept(true) {
  switch (tok) {
    case token_t::PLUS_ASSIGN:
    case token_t::MINUS_ASSIGN:
    case token_t::SLASH_ASSIGN:
    case token_t::STAR_ASSIGN:
    case token_t::XOR_ASSIGN:
    case token_t::OR_ASSIGN:
    case token_t::AND_ASSIGN:
    case token_t::SRLI_ASSIGN:
    case token_t::SLLI_ASSIGN:
      return true;
    default:
      return false;
  }
}

ALWAYS_INLINE constexpr bool is_binary(token_t tok) noexcept(true) {
  switch (tok) {
    case token_t::PLUS:
    case token_t::MINUS:
    case token_t::STAR:
    case token_t::SLASH:
    case token_t::BIT_AND:
    case token_t::BIT_OR:
    case token_t::BIT_XOR:
    case token_t::SLLI:
    case token_t::SRLI:
    case token_t::MOD:
    case token_t::EQ:
    case token_t::NEQ:
    case token_t::GT:
    case token_t::GE:
    case token_t::LT:
    case token_t::LE:
      return true;
    default:
      return false;
  }
}

}// namespace token_traits

struct Token {
  std::string data;
  token_t type = token_t::NONE;
};

#endif// WEAK_LEXER_TOKEN_HPP
