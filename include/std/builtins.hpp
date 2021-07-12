#ifndef BUILTINS_HPP
#define BUILTINS_HPP

#include <functional>
#include <optional>

#include "../ast/ast.hpp"

using builtin_function_t = std::function<std::optional<boost::intrusive_ptr<ast::Object>>(const std::vector<boost::intrusive_ptr<ast::Object>>&)>;

extern const std::unordered_map<std::string, builtin_function_t> builtins;

#endif // BUILTINS_HPP
