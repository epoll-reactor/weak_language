#include "../../include/std/builtins.hpp"
#include "../../src/std/implementation/type_traits.cpp"
#include "../../src/std/implementation/io.cpp"

#include <unordered_map>
#include <string>

extern std::ostream& default_stdout;

const std::unordered_map<std::string, builtin_function_t> builtins
{
    /// type traits
    {"integer?",        is_integer},
    {"float?",          is_float},
    {"string?",         is_string},
    {"array?",          is_array},
    {"procedure?",      is_procedure},
    {"procedure-arity", procedure_arity},
    /// io
    {"print",           print},
    {"println",         println}
};
