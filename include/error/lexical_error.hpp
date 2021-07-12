#ifndef WEAK_ERROR_LEXICAL_ERROR_HPP
#define WEAK_ERROR_LEXICAL_ERROR_HPP

#include <stdexcept>

struct LexicalError : public std::runtime_error
{
    explicit LexicalError(std::string_view err)
        : std::runtime_error(err.data())
    { }
};

#endif //WEAK_ERROR_LEXICAL_ERROR_HPP
