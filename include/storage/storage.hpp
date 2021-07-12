#ifndef WEAK_STORAGE_HPP
#define WEAK_STORAGE_HPP

#include <iostream>

#include <unordered_map>

#include "../crc32.hpp"
#include "../ast/ast.hpp"
#include "../error/semantic_error.hpp"

class Storage
{
    struct StorageRecord
    {
        std::size_t hash;
        std::size_t depth;
        std::string name;
        boost::local_shared_ptr<ast::Object> payload;
    };

public:
    Storage()
    {
        m_inner_scopes.rehash(50);
    }

    void push(std::string_view name, const boost::local_shared_ptr<ast::Object>& value)
    {
        unsigned long hash = crc32::create(name.data());
        m_inner_scopes[hash] = StorageRecord{hash, m_scope_depth, std::string(name.data()), value};
    }

    void overwrite(std::string_view name, const boost::local_shared_ptr<ast::Object>& value)
    {
        if (auto found = internal_find(name))
        {
            found->payload = value;
        }
        else {
            push(name, value);
        }
    }

    boost::local_shared_ptr<ast::Object> lookup(std::string_view name) const
    {
        return find(name)->payload;
    }

    void scope_begin() noexcept
    {
        ++m_scope_depth;
    }

    void scope_end() noexcept
    {
        --m_scope_depth;
    }

private:
    Storage::StorageRecord* find(std::string_view name) const
    {
        auto it = m_inner_scopes.find(crc32::create(name.data()));

        if (it == m_inner_scopes.end() || it->second.depth > m_scope_depth)
            throw SemanticError("Variable not found: " + std::string(name));

        return &it->second;
    }

    Storage::StorageRecord* internal_find(std::string_view name) const noexcept
    {
        auto it = m_inner_scopes.find(crc32::create(name.data()));

        if (it == m_inner_scopes.end() || it->second.depth > m_scope_depth)
            return nullptr;

        return &it->second;
    }

    std::size_t m_scope_depth = 0;
    mutable std::unordered_map <uint64_t, StorageRecord> m_inner_scopes;
};

#endif // WEAK_STORAGE_HPP
