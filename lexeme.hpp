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

    plus_assign,    // +=
    minus_assign,   // -=
    star_assign,    // *=
    slash_assign,   // /=

    num,
    symbol,
    string_literal,
    none,
    end_of_data
};

struct Lexeme
{
    std::string data;
    lexeme_t type = lexeme_t::none;
};


#endif // LEXEME_HPP
