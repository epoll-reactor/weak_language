#ifndef EVAL_ERROR_HPP
#define EVAL_ERROR_HPP

#include <stdexcept>

struct EvalError : public std::runtime_error
{
    explicit EvalError(std::string_view err)
        : std::runtime_error(err.data())
    { }
};

struct RuntimeError : public EvalError
{
    explicit RuntimeError(std::string_view err)
        : EvalError(err.data())
    { }
};

struct TypeError : public EvalError
{
    explicit TypeError(std::string_view err)
        : EvalError(err.data())
    { }
};


#endif // EVAL_ERROR_HPP
