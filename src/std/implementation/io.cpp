#include "../../../include/ast/ast.hpp"
#include "../../../include/cut_last_iterator.hpp"

#include <optional>

extern std::ostream& default_stdout;

inline std::optional<boost::local_shared_ptr<ast::Object>> print(const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) {
  auto print_impl = [&arguments](size_t idx, auto* object) {
    default_stdout << object->value();
    if (idx < arguments.size() - 1) {
      default_stdout << ' ';
    }
  };
  auto type_obj_impl = [](auto* object) {
    default_stdout << '(';
    const auto& fields = object->fields();
    for (const auto& field : cut_last(fields)) {
      print({field.second});
      default_stdout << ", ";
    }
    print({fields.back().second});
    default_stdout << ')';
  };
  auto array_impl = [](auto* object) {
    default_stdout << '[';
    const auto& elements = object->elements();
    for (const auto& field : cut_last(elements)) {
      print({field});
      default_stdout << ", ";
    }
    print({elements.back()});
    default_stdout << ']';
  };
  // clang-format off
  for (size_t i = 0; i < arguments.size(); ++i) {
    ast::Object* pointer = arguments[i].get();
    ast::dynamic_typecase(pointer,
      [&i, &print_impl](ast::Integer* ptr)    { print_impl(i, ptr); },
      [&i, &print_impl](ast::Float* ptr)      { print_impl(i, ptr); },
      [&i, &print_impl](ast::String* ptr)     { print_impl(i, ptr); },
      [    &array_impl](ast::Array* ptr)      { array_impl(   ptr); },
      [ &type_obj_impl](ast::TypeObject* ptr) { type_obj_impl(ptr); }
    );
  }
  // clang-format on
  return std::nullopt;
}

inline std::optional<boost::local_shared_ptr<ast::Object>> println(const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) {
  print(arguments);
  default_stdout << '\n';
  return std::nullopt;
}

#undef DEFINE_STD_FUNCTION