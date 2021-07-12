#ifndef LEXICAL_ERROR_HPP
#define LEXICAL_ERROR_HPP

#include <stdexcept>

struct LexicalError : public std::runtime_error
{
    explicit LexicalError(std::string_view err)
        : std::runtime_error(err.data())
    { }
};

#endif //LEXICAL_ERROR_HPP
