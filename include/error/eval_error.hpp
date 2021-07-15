#ifndef WEAK_ERROR_EVAL_ERROR_HPP
#define WEAK_ERROR_EVAL_ERROR_HPP

#include "../../include/error/common_error.hpp"

struct EvalError : public CommonError
{
public:
    explicit EvalError(std::string_view argument)
        : CommonError("eval_error", argument)
    {}
};

struct RuntimeError : public CommonError
{
public:
    explicit RuntimeError(std::string_view argument)
        : CommonError("runtime_error", argument)
    {}
};

struct TypeError : public CommonError
{
public:
    explicit TypeError(std::string_view argument)
        : CommonError("type_error", argument)
    {}
};

#endif // WEAK_ERROR_EVAL_ERROR_HPP
