#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <unordered_map>

#include "../parser/ast.hpp"
#include "../semantic/semantic_error.hpp"

enum class symbol_context_t
{
    block,
    function,
    function_call,
    variable
};

class SymbolTable
{
public:
    struct SymbolInfo
    {
        std::string name;
        std::size_t depth;
    };

    SymbolInfo lookup(std::string_view name, std::size_t depth = 1)
    {
        if (m_entries.find(name.data()) != m_entries.end())
        {
            return SymbolInfo{name.data(), depth};
        }
        else {
            if (m_outer_table)
            {
                return m_outer_table->lookup(name, ++depth);
            }
            else {
                throw SemanticError("Variable not found");
            }
        }
    }

    void insert(std::string_view name, symbol_context_t traits)
    {
        m_entries.emplace(name, traits);
    }

    void insert(const SymbolTable& scope)
    {
        m_outer_table = std::make_shared<SymbolTable>(scope);
    }

private:
    std::unordered_map<std::string, symbol_context_t> m_entries;
    std::shared_ptr<SymbolTable> m_outer_table;
};

#endif // SYMBOL_TABLE_HPP
