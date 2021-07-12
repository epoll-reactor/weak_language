#ifndef PARSE_ERROR_HPP
#define PARSE_ERROR_HPP

#include <stdexcept>

struct ParseError : public std::runtime_error
{
    explicit ParseError(std::string_view err)
        : std::runtime_error(err.data())
    { }
};

#endif // PARSE_ERROR_HPP
