#include <unordered_map>
#include <string>

#include "../../include/eval/builtins.hpp"

extern std::ostream& default_stdout;

template <typename TargetType>
std::shared_ptr<ast::Object> default_typecheck(const std::string& fun_name, const std::vector<std::shared_ptr<ast::Object>>& arguments)
{
    if (arguments.size() != 1)
        throw EvalError(fun_name + ": 1 argument required, got " + std::to_string(arguments.size()));

    bool is_type = std::dynamic_pointer_cast<TargetType>(arguments[0]).operator bool();

    return std::make_shared<ast::Number>(is_type);
}

std::unordered_map<std::string, builtin_function_t> builtins
{
    {"number?", [](const std::vector<std::shared_ptr<ast::Object>>& arguments) {

        return default_typecheck<ast::Number>("number?", arguments);
    }},
    {"string?", [](const std::vector<std::shared_ptr<ast::Object>>& arguments) {

        return default_typecheck<ast::String>("string?", arguments);
    }},
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
    }}
};