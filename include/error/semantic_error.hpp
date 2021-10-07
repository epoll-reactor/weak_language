#ifndef WEAK_ERROR_SEMANTIC_ERROR_HPP
#define WEAK_ERROR_SEMANTIC_ERROR_HPP

#include "../../include/error/common_error.hpp"

struct SemanticError : public CommonError {
public:
  explicit SemanticError(std::string_view argument)
    : CommonError("semantic_error", argument) {}
  template <typename... Args>
  explicit SemanticError(const char* fmt, Args&&... args)
    : CommonError("semantic_error", format(fmt, std::forward<Args>(args)...)) {}
};

#endif// WEAK_ERROR_SEMANTIC_ERROR_HPP
