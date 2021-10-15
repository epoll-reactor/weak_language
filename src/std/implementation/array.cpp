#include "../../../include/ast/ast.hpp"
#include "../../../include/error/eval_error.hpp"

#include <optional>

template <typename AST>
ALWAYS_INLINE static void perform_assign(ast::Array* array, ast::Integer* index, ast::Object* object) noexcept(false) {
  static_cast<AST*>(array->elements().at(index->value()).get())->value() =
      static_cast<AST*>(object)->value();
}

ALWAYS_INLINE static void perform_insertion(ast::Array* array, ast::Integer* index, boost::local_shared_ptr<ast::Object> object) noexcept(false) {
  array->elements().insert(std::next(array->elements().begin(), index->value()), std::move(object));
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

inline std::optional<boost::local_shared_ptr<ast::Object>> array_replace(const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) noexcept(false) {
  if (arguments.size() != 3) {
    throw EvalError("array-replace: 3 arguments required, got {}", arguments.size());
  }
  auto array = dynamic_cast<ast::Array*>((*arguments.begin()).get());
  auto index = dynamic_cast<ast::Integer*>((*(arguments.begin() + 1)).get());
  auto object = (*(arguments.begin() + 2)).get();
  if (!array || !index) {
    throw EvalError("array-replace: wrong types");
  }
  // clang-format off
  switch (object->ast_type()) {
    case ast::type_t::INTEGER: { perform_assign<ast::Integer>(array, index, object); break; }
    case ast::type_t::FLOAT: { perform_assign<ast::Float>(array, index, object); break; }
    case ast::type_t::STRING: { perform_assign<ast::String>(array, index, object); break; }
    default:
      throw EvalError("array-replace: wrong types");
  }
  // clang-format on
  return std::nullopt;
}

inline std::optional<boost::local_shared_ptr<ast::Object>> array_insert(const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) noexcept(false) {
  if (arguments.size() != 3) {
    throw EvalError("array-insert: 3 arguments required, got {}", arguments.size());
  }
  auto array = dynamic_cast<ast::Array*>((*arguments.begin()).get());
  auto index = dynamic_cast<ast::Integer*>((*(arguments.begin() + 1)).get());
  auto object = *(arguments.begin() + 2);
  if (!array || !index) {
    throw EvalError("array-insert: wrong types");
  }
  // clang-format off
  switch (object->ast_type()) {
    case ast::type_t::INTEGER:
    case ast::type_t::FLOAT:
    case ast::type_t::STRING: {
      perform_insertion(array, index, object);
      break;
    }
    default:
      throw EvalError("array-insert: wrong types");
  }
  // clang-format on
  return std::nullopt;
}

inline std::optional<boost::local_shared_ptr<ast::Object>> array_slice(const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) noexcept(false) {
  if (arguments.size() != 3) {
    throw EvalError("array-slice: 3 arguments required, got {}", arguments.size());
  }
  auto array = dynamic_cast<ast::Array*>((*arguments.begin()).get());
  auto from = dynamic_cast<ast::Integer*>((*(arguments.begin() + 1)).get());
  auto to = dynamic_cast<ast::Integer*>((*(arguments.begin() + 2)).get());
  if (!array || !from || !to) {
    throw EvalError("array-slice: wrong types");
  }
  return boost::make_local_shared<ast::Array>(
      std::vector<boost::local_shared_ptr<ast::Object>>(
          std::next(array->elements().begin(), from->value()),
          std::next(array->elements().begin(), from->value() + to->value())));
}

inline std::optional<boost::local_shared_ptr<ast::Object>> array_merge(const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) noexcept(false) {
  if (arguments.size() != 2) {
    throw EvalError("array-merge: 2 arguments required, got {}", arguments.size());
  }
  auto left_array = dynamic_cast<ast::Array*>((*arguments.begin()).get());
  auto right_array = dynamic_cast<ast::Array*>((*(arguments.begin() + 1)).get());
  if (!left_array || !right_array) {
    throw EvalError("array-merge: wrong types");
  }
  std::vector<boost::local_shared_ptr<ast::Object>> merged;
  merged.reserve(left_array->elements().size() + right_array->elements().size());
  merged.insert(merged.end(), left_array->elements().begin(), left_array->elements().end());
  merged.insert(merged.end(), right_array->elements().begin(), right_array->elements().end());
  return boost::make_local_shared<ast::Array>(std::move(merged));
}

inline std::optional<boost::local_shared_ptr<ast::Object>> array_length(const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) noexcept(false) {
  if (arguments.size() != 1) {
    throw EvalError("array-length: 1 argument required, got {}", arguments.size());
  }
  auto array = dynamic_cast<ast::Array*>((*arguments.begin()).get());
  if (!array) {
    throw EvalError("array-length: wrong type");
  }
  return boost::make_local_shared<ast::Integer>(array->elements().size());
}
