#ifndef WEAK_ERROR_LEXICAL_ERROR_HPP
#define WEAK_ERROR_LEXICAL_ERROR_HPP

#include "../../include/error/common_error.hpp"

struct LexicalError : public CommonError
{
public:
    explicit LexicalError(std::string_view argument)
        : CommonError("lexical_error", argument)
    {}
};

#endif //WEAK_ERROR_LEXICAL_ERROR_HPP
