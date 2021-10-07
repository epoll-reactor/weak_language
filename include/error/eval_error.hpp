#ifndef WEAK_ERROR_EVAL_ERROR_HPP
#define WEAK_ERROR_EVAL_ERROR_HPP

#include "../../include/error/common_error.hpp"

struct EvalError : public CommonError {
public:
  explicit EvalError(std::string_view argument)
    : CommonError("eval_error", argument) {}
  template <typename... Args>
  explicit EvalError(const char* fmt, Args&&... args)
    : CommonError("eval_error", format(fmt, std::forward<Args>(args)...)) {}
};

struct RuntimeError : public CommonError {
public:
  explicit RuntimeError(std::string_view argument)
    : CommonError("runtime_error", argument) {}
  template <typename... Args>
  explicit RuntimeError(const char* fmt, Args&&... args)
    : CommonError("eval_error", format(fmt, std::forward<Args>(args)...)) {}
};

struct TypeError : public CommonError {
public:
  explicit TypeError(std::string_view argument)
    : CommonError("type_error", argument) {}
  template <typename... Args>
  explicit TypeError(const char* fmt, Args&&... args)
    : CommonError("type_error", format(fmt, std::forward<Args>(args)...)) {}
};

#endif// WEAK_ERROR_EVAL_ERROR_HPP
