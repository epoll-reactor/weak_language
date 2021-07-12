#ifndef WEAK_LEXER_LEXEME_HPP
#define WEAK_LEXER_LEXEME_HPP

#include <string>

enum struct lexeme_t
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

inline std::string dispatch_lexeme(lexeme_t o)
{
    switch (o)
    {

        case lexeme_t::dot:             return ".";
        case lexeme_t::comma:           return ",";
        case lexeme_t::plus:            return "+";
        case lexeme_t::minus:           return "-";
        case lexeme_t::inc:             return "++";
        case lexeme_t::dec:             return "--";
        case lexeme_t::star:            return "*";
        case lexeme_t::slash:           return "/";
        case lexeme_t::assign:          return "=";
        case lexeme_t::plus_assign:     return "+=";
        case lexeme_t::minus_assign:    return "-=";
        case lexeme_t::star_assign:     return "*=";
        case lexeme_t::slash_assign:    return "/=";
        case lexeme_t::mod:             return "%";
        case lexeme_t::left_paren:      return "(";
        case lexeme_t::right_paren:     return ")";
        case lexeme_t::left_brace:      return "{";
        case lexeme_t::right_brace:     return "}";
        case lexeme_t::left_box_brace:  return "[";
        case lexeme_t::right_box_brace: return "]";
        case lexeme_t::colon:           return ":";
        case lexeme_t::semicolon:       return ";";
        case lexeme_t::eq:              return "==";
        case lexeme_t::gt:              return ">";
        case lexeme_t::ge:              return ">=";
        case lexeme_t::lt:              return "<";
        case lexeme_t::le:              return "<=";
        case lexeme_t::negation:        return "!";
        case lexeme_t::neq:             return "!=";
        case lexeme_t::srli:            return ">>";
        case lexeme_t::slli:            return "<<";
        case lexeme_t::floating_point:  return "<float>";
        case lexeme_t::num:             return "<number>";
        case lexeme_t::symbol:          return "<symbol>";
        case lexeme_t::string_literal:  return "<string literal>";
        case lexeme_t::none:            return "<none>";
        case lexeme_t::end_of_data:     return "<EOF>";
        case lexeme_t::kw_function_decl:return "<function_decl>";
        case lexeme_t::kw_for:          return "<for>";
        case lexeme_t::kw_while:        return "<while>";
        case lexeme_t::kw_if:           return "<if>";
        case lexeme_t::kw_load:         return "<load>";
        default:                        return "<unknown>";
    }
}

struct Lexeme
{
    std::string data;
    lexeme_t type = lexeme_t::none;
};

#endif // WEAK_LEXER_LEXEME_HPP
