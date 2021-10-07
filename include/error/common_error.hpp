#ifndef WEAK_ERROR_COMMON_ERROR_HPP
#define WEAK_ERROR_COMMON_ERROR_HPP

#include "../format.hpp"

#include <stdexcept>

class CommonError : public std::exception {
public:
  const char* what() const noexcept override {
    return error_.what();
  }

protected:
  explicit CommonError(std::string_view type, std::string_view arg)
    : error_(create(type, arg).data()) {}

  /// @throws std::bad_alloc from `format()`
  static std::string create(std::string_view type, std::string_view arg) noexcept(false) {
    return format("[weak.{}]: {}", type, arg);
  }

private:
  std::runtime_error error_;
};

#endif// WEAK_ERROR_COMMON_ERROR_HPP