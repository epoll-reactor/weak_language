#include "../../include/std/builtins.hpp"

#include "../../include/error/eval_error.hpp"

#include <unordered_map>
#include <string>

extern std::ostream& default_stdout;

template <typename TargetType>
boost::local_shared_ptr<ast::Object> default_typecheck(const std::string& fun_name, const std::vector<boost::local_shared_ptr<ast::Object>>& arguments)
{
    if (arguments.size() != 1) { throw EvalError(fun_name + ": 1 argument required, got " + std::to_string(arguments.size())); }

    return boost::make_local_shared<ast::Integer>(static_cast<bool>(boost::dynamic_pointer_cast<TargetType>(arguments[0])));
}

const std::unordered_map<std::string, builtin_function_t> builtins
{
    {"integer?", [](const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) {

        return default_typecheck<ast::Integer>("integer?", arguments);
    }},
    {"float?", [](const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) {

        return default_typecheck<ast::Float>("float?", arguments);
    }},
    {"string?", [](const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) {

        return default_typecheck<ast::String>("string?", arguments);
    }},
    {"array?", [](const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) {

        return default_typecheck<ast::Array>("array?", arguments);
    }},
    {"procedure?", [](const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) {

        return default_typecheck<ast::Function>("procedure?", arguments);
    }},
    {"procedure-arity", [](const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) {

        if (arguments.size() != 1) { throw EvalError("procedure-arity: 1 argument required, got " + std::to_string(arguments.size())); }

        if (arguments[0]->ast_type() != ast::ast_type_t::FUNCTION) { throw EvalError("procedure-arity: function as argument required"); }

        return boost::make_local_shared<ast::Integer>(boost::static_pointer_cast<ast::Function>(arguments[0])->arguments().size());
    }},
    {"print", [](const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) {
        for (std::size_t i = 0; i < arguments.size(); i++)
        {
            if (auto integral = boost::dynamic_pointer_cast<ast::Integer>(arguments[i]))
            {
                default_stdout << integral->value();

                if (i < arguments.size() - 1) { default_stdout << ' '; }
            }
            else if (auto floating_point = boost::dynamic_pointer_cast<ast::Float>(arguments[i]))
            {
                default_stdout << floating_point->value();

                if (i < arguments.size() - 1) { default_stdout << ' '; }
            }
            else if (auto string = boost::dynamic_pointer_cast<ast::String>(arguments[i])) {

                default_stdout << string->value();

                if (i < arguments.size() - 1) { default_stdout << ' '; }
            }
        }

        return std::nullopt;
    }},
    {"println", [](const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) {
        builtins.at("print")(arguments);

        default_stdout << '\n';

        return std::nullopt;
    }}
};
