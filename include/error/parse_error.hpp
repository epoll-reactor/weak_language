#ifndef WEAK_ERROR_PARSE_ERROR_HPP
#define WEAK_ERROR_PARSE_ERROR_HPP

#include "../../include/error/common_error.hpp"

struct ParseError : public CommonError
{
public:
    explicit ParseError(std::string_view argument)
        : CommonError("parse_error", argument)
    {}
    template <typename... Args>
    explicit ParseError(const char* fmt, Args&&... args)
        : CommonError("parse_error", format(fmt, std::forward<Args>(args)...))
    {}
};

#endif // WEAK_ERROR_PARSE_ERROR_HPP
