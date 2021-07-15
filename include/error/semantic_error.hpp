#ifndef WEAK_ERROR_SEMANTIC_ERROR_HPP
#define WEAK_ERROR_SEMANTIC_ERROR_HPP

#include "../../include/error/common_error.hpp"

struct SemanticError : public CommonError
{
public:
    explicit SemanticError(std::string_view argument)
        : CommonError("semantic_error", argument)
    {}
};

#endif // WEAK_ERROR_SEMANTIC_ERROR_HPP
