#include "../../../include/ast/ast.hpp"
#include "../../../include/error/eval_error.hpp"

#include <optional>

inline std::optional<boost::local_shared_ptr<ast::Object>> array_get(const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) {
  if (arguments.size() != 2) {
    throw EvalError("array-get: 2 arguments required, got " + std::to_string(arguments.size()));
  }
  auto array = dynamic_cast<ast::Array*>((*arguments.begin()).get());
  auto index = dynamic_cast<ast::Integer*>((*++arguments.begin()).get());
  if (!array) {
    throw EvalError("array-get: wrong types");
  }
  if (array->elements().size() <= index->value()) {
    throw EvalError(format("array-get: overflow (index is {}, size is {})", index->value(), array->elements().size()));
  }
  return array->elements().at(index->value());
}

inline std::optional<boost::local_shared_ptr<ast::Object>> array_set(const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) {
  if (arguments.size() != 3) {
    throw EvalError("array-set: 3 arguments required, got " + std::to_string(arguments.size()));
  }
  auto array = dynamic_cast<ast::Array*>((*arguments.begin()).get());
  auto index = dynamic_cast<ast::Integer*>((*(arguments.begin() + 1)).get());
  auto object = (*(arguments.begin() + 2)).get();
  if (!array || !index) {
    throw EvalError("array-set: wrong types");
  }
  auto perform_assign = [&array, &index, &object]<typename AST>() {
    dynamic_cast<AST*>(array->elements().at(index->value()).get())->value() =
        dynamic_cast<AST*>(object)->value();
  };
  // clang-format off
  switch (object->ast_type()) {
    case ast::type_t::INTEGER: { perform_assign.template operator()<ast::Integer>(); break; }
    case ast::type_t::FLOAT: { perform_assign.template operator()<ast::Float>(); break; }
    case ast::type_t::STRING: { perform_assign.template operator()<ast::String>(); break; }
    default:
      throw EvalError("array-set: wrong types");
  }
  // clang-format on
  return std::nullopt;
}