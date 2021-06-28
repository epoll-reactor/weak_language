#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <unordered_map>
#include <vector>

#include "../parser/ast.hpp"
#include "../semantic/semantic_error.hpp"


class Storage
{
    using scope_t = std::unordered_map<std::string, std::shared_ptr<ast::Object>>;

public:
    Storage()
    {
        m_inner_scopes.push_back(scope_t{});
    }

    void push(std::string_view name, std::shared_ptr<ast::Object> value)
    {
        if (std::find_if(m_inner_scopes.begin(), m_inner_scopes.end(), [&name](const scope_t& scope) {
            return scope.contains(name.data());
        }) != m_inner_scopes.end())
        {
            throw SemanticError(std::string("redefinition of ") + name.data());
        }

        m_inner_scopes.back()[name.data()] = std::move(value);
    }

    bool has(std::string_view name)
    {
        if (std::find_if(m_inner_scopes.begin(), m_inner_scopes.end(), [&name](const scope_t& scope) {
            return scope.contains(name.data());
        }) != m_inner_scopes.end())
        {
            return true;
        }

        return false;
    }

    void overwrite(std::string_view name, std::shared_ptr<ast::Object> value)
    {
        if (!has(name))
        {
            push(name, std::move(value));
        }
        else {
            for (auto& scope : m_inner_scopes)
            {
                if (scope.contains(name.data()))
                {
                    scope[name.data()] = std::move(value);
                }
            }
        }
    }

    std::shared_ptr<ast::Object> lookup(std::string_view name) const
    {
        for (const auto& scope : m_inner_scopes)
        {
            if (scope.contains(name.data()))
            {
                return scope.at(name.data());
            }
        }

        throw SemanticError(std::string("Variable not found: ") + name.data());
    }

    void scope_begin()
    {
        m_inner_scopes.push_back(scope_t{});
    }

    void scope_end()
    {
        m_inner_scopes.pop_back();
    }

private:
    std::vector<scope_t> m_inner_scopes;
};

#endif // SYMBOL_TABLE_HPP
