#ifndef WEAK_ERROR_LEXICAL_ERROR_HPP
#define WEAK_ERROR_LEXICAL_ERROR_HPP

#include "../../include/error/common_error.hpp"

struct LexicalError : public CommonError
{
public:
    explicit LexicalError(std::string_view argument)
        : CommonError("lexical_error", argument)
    {}
    template <typename... Args>
    explicit LexicalError(const char* fmt, Args&&... args)
        : CommonError("lexical_error", format(fmt, std::forward<Args>(args)...))
    {}
};

#endif //WEAK_ERROR_LEXICAL_ERROR_HPP
