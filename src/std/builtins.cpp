#include "../../include/std/builtins.hpp"

#include "../../src/std/implementation/array.cpp"
#include "../../src/std/implementation/io.cpp"
#include "../../src/std/implementation/type_traits.cpp"

const std::unordered_map<std::string, builtin_function_t> builtins{
    /// type traits
    {"integer?", is_integer},
    {"float?", is_float},
    {"string?", is_string},
    {"array?", is_array},
    {"procedure?", is_procedure},
    {"procedure-arity", procedure_arity},
    /// arrays
    {"array-get", array_get},
    {"array-replace", array_replace},
    {"array-insert", array_insert},
    {"array-slice", array_slice},
    {"array-length", array_length},
    {"array-merge", array_merge},
    /// io
    {"print", print},
    {"println", println}};
