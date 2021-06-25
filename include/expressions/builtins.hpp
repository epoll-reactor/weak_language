#ifndef BUILTINS_HPP
#define BUILTINS_HPP

#include <unordered_map>
#include <string>
#include <functional>
#include <iostream>
#include <optional>
#include <fstream>

#include "../parser/ast.hpp"
#include "../expressions/eval_error.hpp"

extern std::ostream& default_stdout;

using builtin_function_t = std::function<std::optional<std::shared_ptr<ast::Object>>(const std::vector<std::shared_ptr<ast::Object>>&)>;

static inline std::unordered_map<std::string, builtin_function_t> builtins
{
    {"print", [](const std::vector<std::shared_ptr<ast::Object>>& arguments) {
        for (std::size_t i = 0; i < arguments.size(); i++)
        {
            if (auto num = std::dynamic_pointer_cast<ast::Number>(arguments[i]))
            {
                default_stdout << num->value();

                if (i < arguments.size() - 1)
                    default_stdout << ' ';
            }
            else if (auto string = std::dynamic_pointer_cast<ast::String>(arguments[i])) {

                default_stdout << string->value();

                if (i < arguments.size() - 1)
                    default_stdout << ' ';
            }
        }

        return std::nullopt;
    }},
    {"println", [](const std::vector<std::shared_ptr<ast::Object>>& arguments) {
        builtins["print"](arguments);

        default_stdout << '\n';

        return std::nullopt;
    }},
    {"read_file", [](const std::vector<std::shared_ptr<ast::Object>>& arguments) {
        if (arguments.size() != 1)
            throw EvalError("read_file: wrong arguments count");

        if (auto string = std::dynamic_pointer_cast<ast::String>(arguments[0]))
        {
            std::ifstream file(string->value());

            if (file.fail())
                throw EvalError("Cannot open file");

            return std::make_shared<ast::String>(
                std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>())
            );
        }
        else {
            throw EvalError("String literal as parameter expected");
        }
    }}
};

#endif // BUILTINS_HPP
