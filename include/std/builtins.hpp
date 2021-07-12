#ifndef WEAK_STD_BUILTINS_HPP
#define WEAK_STD_BUILTINS_HPP

#include <functional>
#include <optional>

#include "../ast/ast.hpp"

using builtin_function_t = std::function<std::optional<boost::local_shared_ptr<ast::Object>>(const std::vector<boost::local_shared_ptr<ast::Object>>&)>;

extern const std::unordered_map<std::string, builtin_function_t> builtins;

#endif // WEAK_STD_BUILTINS_HPP
