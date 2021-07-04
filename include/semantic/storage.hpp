#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <unordered_map>
#include <vector>

#include "../parser/ast.hpp"
#include "../semantic/semantic_error.hpp"


class Storage
{
    struct StorageRecord
    {
        std::size_t depth;
        std::shared_ptr<ast::Object> payload;
    };

public:
    void push(std::string_view name, const std::shared_ptr<ast::Object>& value)
    {
        m_inner_scopes[name.data()] = StorageRecord{m_scope_depth, value};
    }

    Storage::StorageRecord* has(std::string_view name) const
    {
        auto it = m_inner_scopes.find(name.data());

        if (it == m_inner_scopes.end() || it->second.depth > m_scope_depth)
            return nullptr;

        return &it->second;
    }

    void overwrite(std::string_view name, const std::shared_ptr<ast::Object>& value)
    {
        auto found_data = has(name);

        if (found_data)
        {
            found_data->payload = value;
        }
        else {
            push(name, std::move(value));
        }
    }

    std::shared_ptr<ast::Object> lookup(std::string_view name) const
    {
        auto found_data = has(name);

        if (found_data)
        {
            return found_data->payload;
        }
        else {
            throw SemanticError("Variable not found: " + std::string(name));
        }
    }

    void scope_begin()
    {
        ++m_scope_depth;
    }

    void scope_end()
    {
        --m_scope_depth;
    }

private:
    std::size_t m_scope_depth = 0;
    mutable std::unordered_map<std::string, StorageRecord> m_inner_scopes;
};

#endif // SYMBOL_TABLE_HPP
