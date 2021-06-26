#ifndef BUILTINS_HPP
#define BUILTINS_HPP

#include <functional>
#include <optional>

#include "../parser/ast.hpp"
#include "../eval/eval_error.hpp"

using builtin_function_t = std::function<std::optional<std::shared_ptr<ast::Object>>(const std::vector<std::shared_ptr<ast::Object>>&)>;

extern std::unordered_map<std::string, builtin_function_t> builtins;

#endif // BUILTINS_HPP
