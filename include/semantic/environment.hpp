#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <unordered_map>
#include <stack>

#include "../parser/ast.hpp"
#include "../semantic/semantic_error.hpp"

class Environment
{
    using scope_t = std::unordered_map<std::string, std::string>;

public:
    Environment()
    {
        m_inner_scopes.push(scope_t{});
    }

    void push(std::string_view name, std::string_view value)
    {
        if (m_inner_scopes.empty())
            throw std::runtime_error("At least one symbol table must be loaded before push");

        m_inner_scopes.top().emplace(name, value);
    }

    bool lookup(std::string_view name)
    {
        auto scopes_copy = m_inner_scopes;

        while (!scopes_copy.empty())
        {
            if (scopes_copy.top().contains(name.data()))
                return true;

            scopes_copy.pop();
        }

        return false;
    }

    void scope_begin()
    {
        m_inner_scopes.push(scope_t{});
    }

    void scope_end()
    {
        m_inner_scopes.pop();
    }

private:
    std::stack<scope_t> m_inner_scopes;
};

#endif // SYMBOL_TABLE_HPP
