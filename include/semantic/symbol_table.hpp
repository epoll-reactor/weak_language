#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <unordered_map>

#include "../parser/ast.hpp"

enum class symbol_context_t
{

};

class SymbolTable
{
public:
    using entry_info_t = std::pair<symbol_context_t, symbol_context_t>;

private:
    std::unordered_map<std::string, std::string> m_entries;
};

#endif // SYMBOL_TABLE_HPP
