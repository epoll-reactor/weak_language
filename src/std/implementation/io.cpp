#include "../../../include/ast/ast.hpp"
#include "../../../include/cut_last_iterator.hpp"
#include "../../../include/error/error.hpp"

#include <optional>
#include <ranges>

extern std::ostream& default_stdout;

inline std::optional<boost::local_shared_ptr<ast::Object>> print(const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) {
  for (size_t i = 0; i < arguments.size(); i++) {
    // clang-format off
    auto integral       = dynamic_cast<ast::Integer*>(arguments[i].get());
    auto floating_point = dynamic_cast<ast::Float*>(arguments[i].get());
    auto string         = dynamic_cast<ast::String*>(arguments[i].get());
    auto type_object    = dynamic_cast<ast::TypeObject*>(arguments[i].get());
    // clang-format on
    auto print_impl = [&arguments](size_t idx, auto* object) {
      default_stdout << object->value();
      if (idx < arguments.size() - 1) {
        default_stdout << ' ';
      }
    };
    auto type_object_print_impl = [](auto* object) {
      default_stdout << '(';
      const auto& fields = object->fields();
      for (const auto& field : cut_last(fields)) {
        print({field.second});
        default_stdout << ", ";
      }
      print({fields.back().second});
      default_stdout << ')';
    };
    if (integral) {
      print_impl(i, integral);
    }
    if (floating_point) {
      print_impl(i, floating_point);
    }
    if (string) {
      print_impl(i, string);
    }
    if (type_object) {
      type_object_print_impl(type_object);
    }
  }
  return std::nullopt;
}

inline std::optional<boost::local_shared_ptr<ast::Object>> println(const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) {
  print(arguments);
  default_stdout << '\n';
  return std::nullopt;
}

#undef DEFINE_STD_FUNCTION