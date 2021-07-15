#ifndef WEAK_ERROR_PARSE_ERROR_HPP
#define WEAK_ERROR_PARSE_ERROR_HPP

#include "../../include/error/common_error.hpp"

struct ParseError : public CommonError
{
public:
    explicit ParseError(std::string_view argument)
        : CommonError("parse_error", argument)
    {}
};

#endif // WEAK_ERROR_PARSE_ERROR_HPP
