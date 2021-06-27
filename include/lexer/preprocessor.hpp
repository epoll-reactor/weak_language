#ifndef PREPROCESSOR_HPP
#define PREPROCESSOR_HPP

#include <string>

/// @brief  replace `load \"filename\";` statement with filename contents
/// @return recursively expanded file contents
std::string preprocess_file(std::string_view filename);

#endif // PREPROCESSOR_HPP
