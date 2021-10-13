#include "../../include/storage/storage.hpp"

#include "../../include/ast/ast.hpp"
#include "../../include/crc32.hpp"

// clang-format off
static inline std::string ascii_to_lower(std::string_view str) noexcept(false) {
  std::string converted;
  converted.reserve(str.length());
  for (char c : str) {
    converted.push_back((c <= 'Z' && c >= 'A')
      ? static_cast<char>(c - ('Z' - 'z'))
      : static_cast<char>(c));
  }
  return converted;
}
// clang-format on

void Storage::push(std::string_view name, boost::local_shared_ptr<ast::Object> value) noexcept(false) {
  const size_t hash = crc32::create(ascii_to_lower(name).data());
  StorageRecord record{scope_depth_, std::move(value)};
  inner_scopes_[hash] = std::move(record);
}

void Storage::overwrite(std::string_view name, boost::local_shared_ptr<ast::Object> value) noexcept(false) {
  if (const auto found = find(ascii_to_lower(name))) {
    found->payload = std::move(value);
  } else {
    push(name, std::move(value));
  }
}

const boost::local_shared_ptr<ast::Object>& Storage::lookup(std::string_view name) const noexcept(false) {
  const std::string lower_name = ascii_to_lower(name);
  if (auto pointer = find(lower_name)) {
    return pointer->payload;
  }
  throw EvalError("Variable not found: {}", lower_name);
}

boost::local_shared_ptr<ast::Object>& Storage::lookup(std::string_view name) noexcept(false) {
  const std::string lower_name = ascii_to_lower(name);
  if (auto pointer = find(lower_name)) {
    return pointer->payload;
  }
  throw EvalError("Variable not found: {}", lower_name);
}

Storage::StorageRecord* Storage::find(std::string_view name) const noexcept(true) {
  const auto it = inner_scopes_.find(crc32::create(ascii_to_lower(name).data()));
  if (it == inner_scopes_.end() || it->second.depth > scope_depth_) {
    return nullptr;
  }
  return &it->second;
}
