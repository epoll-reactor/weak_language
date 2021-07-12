#include "../../include/storage/storage.hpp"

Storage::Storage()
{
    m_inner_scopes.rehash(50);
}

void Storage::push(std::string_view name, const boost::local_shared_ptr<ast::Object>& value)
{
    unsigned long hash = crc32::create(name.data());
    m_inner_scopes[hash] = StorageRecord{hash, m_scope_depth, std::string(name.data()), value};
}

void Storage::overwrite(std::string_view name, const boost::local_shared_ptr<ast::Object>& value)
{
    if (auto found = internal_find(name))
    {
        found->payload = value;
    }
    else {
        push(name, value);
    }
}

boost::local_shared_ptr<ast::Object> Storage::lookup(std::string_view name) const
{
    return find(name)->payload;
}

void Storage::scope_begin() noexcept
{
    ++m_scope_depth;
}

void Storage::scope_end() noexcept
{
    --m_scope_depth;
}

Storage::StorageRecord* Storage::find(std::string_view name) const
{
    auto it = m_inner_scopes.find(crc32::create(name.data()));

    if (it == m_inner_scopes.end() || it->second.depth > m_scope_depth)
        throw SemanticError("Variable not found: " + std::string(name));

    return &it->second;
}

Storage::StorageRecord* Storage::internal_find(std::string_view name) const noexcept
{
    auto it = m_inner_scopes.find(crc32::create(name.data()));

    if (it == m_inner_scopes.end() || it->second.depth > m_scope_depth)
        return nullptr;

    return &it->second;
}