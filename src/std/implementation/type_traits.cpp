#include "../../../include/ast/ast.hpp"
#include "../../../include/error/error.hpp"

#include <optional>

template <typename TargetType>
static boost::local_shared_ptr<ast::Object> default_typecheck(const std::string& fun_name, const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) {
  if (arguments.size() != 1) {
    throw EvalError(fun_name + ": 1 argument required, got " + std::to_string(arguments.size()));
  }
  return boost::make_local_shared<ast::Integer>(static_cast<bool>(boost::dynamic_pointer_cast<TargetType>(arguments[0])));
}

inline std::optional<boost::local_shared_ptr<ast::Object>> is_integer(const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) {
  return default_typecheck<ast::Integer>("integer?", arguments);
}
inline std::optional<boost::local_shared_ptr<ast::Object>> is_float(const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) {
  return default_typecheck<ast::Float>("float?", arguments);
}
inline std::optional<boost::local_shared_ptr<ast::Object>> is_string(const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) {
  return default_typecheck<ast::String>("string?", arguments);
}
inline std::optional<boost::local_shared_ptr<ast::Object>> is_procedure(const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) {
  return default_typecheck<ast::Lambda>("procedure?", arguments);
}
inline std::optional<boost::local_shared_ptr<ast::Object>> is_array(const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) {
  return default_typecheck<ast::Array>("array?", arguments);
}
inline std::optional<boost::local_shared_ptr<ast::Object>> procedure_arity(const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) {
  if (arguments.size() != 1) {
    throw EvalError("procedure-arity: 1 argument required, got " + std::to_string(arguments.size()));
  }
  if (arguments[0]->ast_type() != ast::type_t::LAMBDA) {
    throw EvalError("procedure-arity: function as argument required");
  }
  return boost::make_local_shared<ast::Integer>(boost::static_pointer_cast<ast::Lambda>(arguments[0])->arguments().size());
}
