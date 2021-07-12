#ifndef WEAK_ERROR_SEMANTIC_ERROR_HPP
#define WEAK_ERROR_SEMANTIC_ERROR_HPP

#include <stdexcept>

struct SemanticError : public std::runtime_error
{
    explicit SemanticError(std::string_view err)
        : std::runtime_error(err.data())
    { }
};


#endif // WEAK_ERROR_SEMANTIC_ERROR_HPP
