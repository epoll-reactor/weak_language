#include "../../../include/ast/ast.hpp"
#include "../../../include/error/error.hpp"

#include <optional>

extern std::ostream& default_stdout;

#define DEFINE_STD_FUNCTION(name) \
    inline std::optional<boost::local_shared_ptr<ast::Object>> name(const std::vector<boost::local_shared_ptr<ast::Object>>& arguments)

DEFINE_STD_FUNCTION(print)
{
    for (std::size_t i = 0; i < arguments.size(); i++)
    {
        if (auto integral = boost::dynamic_pointer_cast<ast::Integer>(arguments[i])) {
            default_stdout << integral->value();
            if (i < arguments.size() - 1) { default_stdout << ' '; }
        }
        else if (auto floating_point = boost::dynamic_pointer_cast<ast::Float>(arguments[i])) {
            default_stdout << floating_point->value();
            if (i < arguments.size() - 1) { default_stdout << ' '; }
        }
        else if (auto string = boost::dynamic_pointer_cast<ast::String>(arguments[i])) {
            default_stdout << string->value();
            if (i < arguments.size() - 1) { default_stdout << ' '; }
        }
        else if (auto type_object = boost::dynamic_pointer_cast<ast::TypeObject>(arguments[i])) {
            default_stdout  << '(';
            const auto& fields = type_object->fields();
            size_t field_iterator = 0;
            for (const auto& field : type_object->fields()) {
                print({field.second});
                if (field_iterator < fields.size() - 1) {
                    default_stdout << ", ";
                }
                ++field_iterator;
            }
            default_stdout  << ')';
        }
    }

    return std::nullopt;
}

DEFINE_STD_FUNCTION(println)
{
    print(arguments);

    default_stdout << '\n';

    return std::nullopt;
}

#undef DEFINE_STD_FUNCTION