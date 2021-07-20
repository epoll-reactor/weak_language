#ifndef WEAK_STORAGE_HPP
#define WEAK_STORAGE_HPP

#include "../common_defs.hpp"
#include "../error/eval_error.hpp"

#include <boost/smart_ptr/local_shared_ptr.hpp>

#include <map>

namespace ast { class Object; }

class Storage
{
    struct StorageRecord
    {
        std::size_t depth;
        std::string name;
        boost::local_shared_ptr<ast::Object> payload;
    };

public:
    /// @throws std::bad_alloc
    void push(std::string_view name, const boost::local_shared_ptr<ast::Object>& value) noexcept(false);

    /// @throws std::bad_alloc
    void overwrite(std::string_view name, const boost::local_shared_ptr<ast::Object>& value) noexcept(false);

    /// @throws EvalError if variable not found
    /// @throws std::bad_alloc
    const boost::local_shared_ptr<ast::Object>& lookup(std::string_view name) const noexcept(false);

    ALWAYS_INLINE void scope_begin() noexcept(true);
    ALWAYS_INLINE void scope_end() noexcept(true);

private:
    Storage::StorageRecord* find(std::string_view name) const noexcept(true);

    std::size_t m_scope_depth = 0;
    mutable std::map<uint64_t, StorageRecord> m_inner_scopes;
};

void Storage::scope_begin() noexcept(true) { ++m_scope_depth; }
void Storage::scope_end() noexcept(true) { --m_scope_depth; }

#endif // WEAK_STORAGE_HPP
