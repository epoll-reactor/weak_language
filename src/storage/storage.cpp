#include "../../include/storage/storage.hpp"
#include "../../include/ast/ast.hpp"
#include "../../include/crc32.hpp"

void Storage::push(std::string_view name, boost::local_shared_ptr<ast::Object> value) noexcept(false)
{
    unsigned long hash = crc32::create(name.data());
    m_inner_scopes[hash] = StorageRecord{m_scope_depth, std::string(name.data()), std::move(value)};
}

void Storage::overwrite(std::string_view name, boost::local_shared_ptr<ast::Object> value) noexcept(false)
{
    if (auto found = find(name))
    {
        found->payload = std::move(value);
    }
    else {
        push(name, std::move(value));
    }
}

const boost::local_shared_ptr<ast::Object>& Storage::lookup(std::string_view name) const noexcept(false)
{
    auto it = m_inner_scopes.find(crc32::create(name.data()));

    if (it == m_inner_scopes.end() || it->second.depth > m_scope_depth)
        throw EvalError("Variable not found: " + std::string(name));

    return it->second.payload;
}

Storage::StorageRecord* Storage::find(std::string_view name) const noexcept(true)
{
    auto it = m_inner_scopes.find(crc32::create(name.data()));

    if (it == m_inner_scopes.end() || it->second.depth > m_scope_depth)
        return nullptr;

    return &it->second;
}
