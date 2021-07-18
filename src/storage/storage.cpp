#include "../../include/storage/storage.hpp"
#include "../../include/ast/ast.hpp"
#include "../../include/crc32.hpp"

void Storage::push(std::string_view name, const boost::local_shared_ptr<ast::Object>& value) noexcept(false)
{
    const unsigned long hash = crc32::create(name.data());
    m_inner_scopes[hash] = StorageRecord{m_scope_depth, std::string(name.data()), value};
}

void Storage::overwrite(std::string_view name, const boost::local_shared_ptr<ast::Object>& value) noexcept(false)
{
    if (const auto found = find(name)) {
        found->payload = value;
    } else {
        push(name, value);
    }
}

const boost::local_shared_ptr<ast::Object>& Storage::lookup(std::string_view name) const noexcept(false)
{
    const auto it = m_inner_scopes.find(crc32::create(name.data()));

    if (it == m_inner_scopes.end() || it->second.depth > m_scope_depth) {
        throw EvalError("Variable not found: {}", std::string(name));
    }

    return it->second.payload;
}

Storage::StorageRecord* Storage::find(std::string_view name) const noexcept(true)
{
    const auto it = m_inner_scopes.find(crc32::create(name.data()));

    if (it == m_inner_scopes.end() || it->second.depth > m_scope_depth) {
        return nullptr;
    }

    return &it->second;
}
