#ifndef SEMANTIC_ERROR_HPP
#define SEMANTIC_ERROR_HPP

#include <stdexcept>

struct SemanticError : public std::runtime_error
{
    explicit SemanticError(std::string_view err)
        : std::runtime_error(err.data())
    { }
};


#endif // SEMANTIC_ERROR_HPP
