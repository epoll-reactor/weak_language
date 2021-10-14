#include "../../../include/ast/ast.hpp"
#include "../../../include/error/eval_error.hpp"

#include <optional>

template <typename AST>
ALWAYS_INLINE static void perform_assign(ast::Array* array, ast::Integer* index, ast::Object* object) noexcept(false) {
  static_cast<AST*>(array->elements().at(index->value()).get())->value() =
      static_cast<AST*>(object)->value();
}

inline std::optional<boost::local_shared_ptr<ast::Object>> array_get(const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) noexcept(false) {
  if (arguments.size() != 2) {
    throw EvalError("array-get: 2 arguments required, got {}", arguments.size());
  }
  auto array = dynamic_cast<ast::Array*>((*arguments.begin()).get());
  auto index = dynamic_cast<ast::Integer*>((*++arguments.begin()).get());
  if (!array || !index) {
    throw EvalError("array-get: wrong types");
  }
  if (array->elements().size() <= index->value()) {
    throw EvalError("array-get: overflow (index is {}, size is {})", index->value(), array->elements().size());
  }
  return array->elements().at(index->value());
}

inline std::optional<boost::local_shared_ptr<ast::Object>> array_set(const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) noexcept(false) {
  if (arguments.size() != 3) {
    throw EvalError("array-set: 3 arguments required, got {}", arguments.size());
  }
  auto array = dynamic_cast<ast::Array*>((*arguments.begin()).get());
  auto index = dynamic_cast<ast::Integer*>((*(arguments.begin() + 1)).get());
  auto object = (*(arguments.begin() + 2)).get();
  if (!array || !index) {
    throw EvalError("array-set: wrong types");
  }
  // clang-format off
  switch (object->ast_type()) {
    case ast::type_t::INTEGER: { perform_assign<ast::Integer>(array, index, object); break; }
    case ast::type_t::FLOAT: { perform_assign<ast::Float>(array, index, object); break; }
    case ast::type_t::STRING: { perform_assign<ast::String>(array, index, object); break; }
    default:
      throw EvalError("array-set: wrong types");
  }
  // clang-format on
  return std::nullopt;
}