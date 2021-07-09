#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <iostream>

#include <unordered_map>

#include <boost/intrusive_ptr.hpp>

#include "crc32.hpp"

#include "../parser/ast.hpp"
#include "../semantic/semantic_error.hpp"

class Storage
{
    struct StorageRecord
    {
        std::size_t hash;
        std::size_t depth;
        std::string name;
        boost::intrusive_ptr<ast::Object> payload;
    };

public:
    Storage()
    {
        m_inner_scopes.rehash(50);
    }

    void push(std::string_view name, const boost::intrusive_ptr<ast::Object>& value)
    {
        unsigned long hash = crc32::create(name.data());
        m_inner_scopes[hash] = StorageRecord{hash, m_scope_depth, std::string(name.data()), value};
    }

    void overwrite(std::string_view name, const boost::intrusive_ptr<ast::Object>& value)
    {
        try {
            find(name)->payload = value;

        }  catch (SemanticError&) {

            push(name, value);
        }
    }

    boost::intrusive_ptr<ast::Object> lookup(std::string_view name) const
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

    std::size_t m_scope_depth = 0;
    mutable std::unordered_map <uint64_t, StorageRecord> m_inner_scopes;
};

#endif // SYMBOL_TABLE_HPP
