#include "../../include/storage/storage.hpp"
#include "../../include/ast/ast.hpp"
#include "../../include/crc32.hpp"

void Storage::push(std::string_view name, boost::local_shared_ptr<ast::Object> value) noexcept(false)
{
    const size_t hash = crc32::create(name.data());
    const auto it = m_inner_scopes.equal_range(hash);
    StorageRecord record { m_scope_depth, std::move(value) };

    if (it.first == it.second) {
        m_inner_scopes.emplace_hint(it.first, hash, std::move(record));
    } else {
        m_inner_scopes[hash] = std::move(record);
    }
}

void Storage::overwrite(std::string_view name, boost::local_shared_ptr<ast::Object> value) noexcept(false)
{
    if (const auto found = find(name)) {
        found->payload = std::move(value);
    } else {
        push(name, std::move(value));
    }
}

const boost::local_shared_ptr<ast::Object>& Storage::lookup(std::string_view name) const noexcept(false)
{
    if (auto pointer = find(name)) { return pointer->payload; }
    throw EvalError("Variable not found: {}", name);
}

Storage::StorageRecord* Storage::find(std::string_view name) const noexcept(true)
{
    const auto it = m_inner_scopes.find(crc32::create(name.data()));

    if (it == m_inner_scopes.end() || it->second.depth > m_scope_depth) {
        return nullptr;
    }

    return &it->second;
}
