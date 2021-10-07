#ifndef WEAK_LEXER_PREPROCESSOR_HPP
#define WEAK_LEXER_PREPROCESSOR_HPP

#include <string>

/// @throws std::runtime_error if file wasn't opened
/// @throws std::bad_alloc
/// @throws std::regex_error
/// @brief  replace `load \"filename\";` statement with filename contents
/// @return recursively expanded file contents
std::string preprocess_file(std::string_view filename) noexcept(false);

#endif// WEAK_LEXER_PREPROCESSOR_HPP
