#ifndef WEAK_LEXER_TOKEN_HPP
#define WEAK_LEXER_TOKEN_HPP

#include <string>

enum struct token_t
{
    dot,            // .
    comma,          // ,
    negation,       // !

    gt,             // >
    ge,             // >=
    lt,             // <

    le,             // <=
    eq,             // ==
    neq,            // !=

    mod,            // %
    plus,           // +
    minus,          // -
    inc,            // ++
    dec,            // --
    star,           // *
    slash,          // /
    assign,         // =

    slli,           // <<
    srli,           // >>

    bit_and,        // &
    bit_or,         // |
    bit_xor,        // ^

    logical_and,    // &&
    logical_or,     // ||

    plus_assign,    // +=
    minus_assign,   // -=
    star_assign,    // *=
    slash_assign,   // /=
    slli_assign,    // <<=
    srli_assign,    // >>=
    and_assign,     // &=
    or_assign,      // |=
    xor_assign,     // ^=

    left_paren,     // (
    right_paren,    // )
    left_brace,     // {
    right_brace,    // }
    left_box_brace, // [
    right_box_brace,// ]

    colon,          // :
    semicolon,      // ;

    kw_for,
    kw_while,
    kw_if,
    kw_else,
    kw_return,
    kw_function_decl,
    kw_define_type,
    kw_load,

    num,
    floating_point,
    symbol,
    string_literal,
    none,
    end_of_data
};

inline std::string dispatch_token(token_t o)
{
    switch (o)
    {

        case token_t::dot:             return ".";
        case token_t::comma:           return ",";
        case token_t::plus:            return "+";
        case token_t::minus:           return "-";
        case token_t::inc:             return "++";
        case token_t::dec:             return "--";
        case token_t::star:            return "*";
        case token_t::slash:           return "/";
        case token_t::assign:          return "=";
        case token_t::plus_assign:     return "+=";
        case token_t::minus_assign:    return "-=";
        case token_t::star_assign:     return "*=";
        case token_t::slash_assign:    return "/=";
        case token_t::mod:             return "%";
        case token_t::left_paren:      return "(";
        case token_t::right_paren:     return ")";
        case token_t::left_brace:      return "{";
        case token_t::right_brace:     return "}";
        case token_t::left_box_brace:  return "[";
        case token_t::right_box_brace: return "]";
        case token_t::colon:           return ":";
        case token_t::semicolon:       return ";";
        case token_t::eq:              return "==";
        case token_t::gt:              return ">";
        case token_t::ge:              return ">=";
        case token_t::lt:              return "<";
        case token_t::le:              return "<=";
        case token_t::negation:        return "!";
        case token_t::neq:             return "!=";
        case token_t::srli:            return ">>";
        case token_t::slli:            return "<<";
        case token_t::floating_point:  return "<float>";
        case token_t::num:             return "<number>";
        case token_t::symbol:          return "<symbol>";
        case token_t::string_literal:  return "<string literal>";
        case token_t::none:            return "<none>";
        case token_t::end_of_data:     return "<EOF>";
        case token_t::kw_function_decl:return "<function_decl>";
        case token_t::kw_for:          return "<for>";
        case token_t::kw_while:        return "<while>";
        case token_t::kw_if:           return "<if>";
        case token_t::kw_load:         return "<load>";
        default:                       return "<unknown>";
    }
}

struct Lexeme
{
    std::string data;
    token_t type = token_t::none;
};

#endif // WEAK_LEXER_TOKEN_HPP
