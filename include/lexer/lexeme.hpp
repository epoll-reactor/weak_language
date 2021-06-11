#ifndef LEXEME_HPP
#define LEXEME_HPP

#include <string>


enum struct lexeme_t
{
    dot,            // .
    comma,          // ,

    plus,           // +
    minus,          // -
    inc,            // ++
    dec,            // --
    star,           // *
    slash,          // /
    assign,         // =

    equal,          // ==
    plus_assign,    // +=
    minus_assign,   // -=
    star_assign,    // *=
    slash_assign,   // /=

    left_paren,     // (
    right_paren,    // )
    left_brace,     // {
    right_brace,    // }

    colon,          // :
    semicolon,      // ;

    kw_for,
    kw_while,
    kw_if,
    kw_else,
    kw_return,

    num,
    symbol,
    string_literal,
    none,
    end_of_data
};

std::string dispatch_lexeme(lexeme_t o)
{
    switch (o)
    {

        case lexeme_t::dot:
            return ".";
        case lexeme_t::comma:
            return ",";
        case lexeme_t::plus:
            return "+";
        case lexeme_t::minus:
            return "-";
        case lexeme_t::inc:
            return "++";
        case lexeme_t::dec:
            return "--";
        case lexeme_t::star:
            return "*";
        case lexeme_t::slash:
            return "/";
        case lexeme_t::assign:
            return "=";
        case lexeme_t::plus_assign:
            return "+=";
        case lexeme_t::minus_assign:
            return "-=";
        case lexeme_t::star_assign:
            return "*=";
        case lexeme_t::slash_assign:
            return "/=";
        case lexeme_t::left_paren:
            return "(";
        case lexeme_t::right_paren:
            return ")";
        case lexeme_t::left_brace:
            return "{";
        case lexeme_t::right_brace:
            return "}";
        case lexeme_t::colon:
            return ":";
        case lexeme_t::semicolon:
            return ";";
        case lexeme_t::num:
            return "<number>";
        case lexeme_t::symbol:
            return "<symbol>";
        case lexeme_t::string_literal:
            return "<string literal>";
        case lexeme_t::none:
            return "<none>";
        case lexeme_t::equal:
            return "==";
        case lexeme_t::end_of_data:
            return "<EOF>";
        default:
            return "<unknown>";
    }
}

struct Lexeme
{
    std::string data;
    lexeme_t type = lexeme_t::none;
};

#endif // LEXEME_HPP
