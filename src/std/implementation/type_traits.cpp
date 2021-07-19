#include "../../../include/ast/ast.hpp"
#include "../../../include/error/error.hpp"

#include <optional>

#define DEFINE_STD_FUNCTION(name) \
    inline std::optional<boost::local_shared_ptr<ast::Object>> name(const std::vector<boost::local_shared_ptr<ast::Object>>& arguments)

template <typename TargetType>
boost::local_shared_ptr<ast::Object> default_typecheck(const std::string& fun_name, const std::vector<boost::local_shared_ptr<ast::Object>>& arguments)
{
    if (arguments.size() != 1) { throw EvalError(fun_name + ": 1 argument required, got " + std::to_string(arguments.size())); }

    return boost::make_local_shared<ast::Integer>(static_cast<bool>(boost::dynamic_pointer_cast<TargetType>(arguments[0])));
}

DEFINE_STD_FUNCTION(is_integer)
{
    return default_typecheck<ast::Integer>("integer?", arguments);
}

DEFINE_STD_FUNCTION(is_float)
{
    return default_typecheck<ast::Float>("float?", arguments);
}

DEFINE_STD_FUNCTION(is_string)
{
    return default_typecheck<ast::String>("string?", arguments);
}

DEFINE_STD_FUNCTION(is_procedure)
{
    return default_typecheck<ast::Function>("procedure?", arguments);
}

DEFINE_STD_FUNCTION(is_array)
{
    return default_typecheck<ast::Array>("array?", arguments);
}

DEFINE_STD_FUNCTION(procedure_arity)
{
    if (arguments.size() != 1) { throw EvalError("procedure-arity: 1 argument required, got " + std::to_string(arguments.size())); }

    if (arguments[0]->ast_type() != ast::ast_type_t::FUNCTION) { throw EvalError("procedure-arity: function as argument required"); }

    return boost::make_local_shared<ast::Integer>(boost::static_pointer_cast<ast::Function>(arguments[0])->arguments().size());
}

#undef DEFINE_STD_FUNCTION