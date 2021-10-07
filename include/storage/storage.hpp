#ifndef WEAK_STORAGE_HPP
#define WEAK_STORAGE_HPP

#include "../common_defs.hpp"
#include "../error/eval_error.hpp"

#include <boost/smart_ptr/local_shared_ptr.hpp>
#include <map>

namespace ast {
class Object;
}

class Storage {
  struct StorageRecord {
    size_t depth;
    boost::local_shared_ptr<ast::Object> payload;
  };

public:
  /// @throws std::bad_alloc
  void push(std::string_view name, boost::local_shared_ptr<ast::Object> value) noexcept(false);

  /// @throws std::bad_alloc
  void overwrite(std::string_view name, boost::local_shared_ptr<ast::Object> value) noexcept(false);

  /// @throws EvalError if variable not found
  /// @throws std::bad_alloc
  const boost::local_shared_ptr<ast::Object>& lookup(std::string_view name) const noexcept(false);

  ALWAYS_INLINE void scope_begin() noexcept(true);
  ALWAYS_INLINE void scope_end() noexcept(true);

private:
  Storage::StorageRecord* find(std::string_view name) const noexcept(true);

  size_t scope_depth_ = 0;
  mutable std::unordered_map<size_t, StorageRecord> inner_scopes_;
};

void Storage::scope_begin() noexcept(true) {
  ++scope_depth_;
}

void Storage::scope_end() noexcept(true) {
  for (auto it = inner_scopes_.begin(); it != inner_scopes_.end(); ++it) {
    if (it->second.depth == scope_depth_) {
      inner_scopes_.erase(it);
      break;
    }
  }
  --scope_depth_;
}

#endif// WEAK_STORAGE_HPP
