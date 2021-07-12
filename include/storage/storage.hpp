#ifndef WEAK_STORAGE_HPP
#define WEAK_STORAGE_HPP

#include "../crc32.hpp"
#include "../ast/ast.hpp"
#include "../error/semantic_error.hpp"

#include <unordered_map>

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
    Storage();

    void push(std::string_view name, const boost::local_shared_ptr<ast::Object>& value);
    void overwrite(std::string_view name, const boost::local_shared_ptr<ast::Object>& value);
    boost::local_shared_ptr<ast::Object> lookup(std::string_view name) const;

    void scope_begin() noexcept;
    void scope_end() noexcept;

private:
    Storage::StorageRecord* find(std::string_view name) const;
    Storage::StorageRecord* internal_find(std::string_view name) const noexcept;

    std::size_t m_scope_depth = 0;
    mutable std::unordered_map <uint64_t, StorageRecord> m_inner_scopes;
};

#endif // WEAK_STORAGE_HPP
