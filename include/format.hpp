#ifndef WEAK_FORMAT_HPP
#define WEAK_FORMAT_HPP

#include <string>
#include <array>

/// @throws std::length_error from std::vector::reserve
/// @throws std::bad_alloc from std::string
template <typename... Args>
static inline std::string format(std::string_view data, Args&&... args) noexcept(false)
{
    if (data.empty()) { return {}; }

    std::string formatted;
    formatted.reserve(data.size() + (/*average_word_size=*/7 * sizeof...(args)));
    auto pack_one = [](auto&& argument) {
        if constexpr (std::is_integral_v<std::decay_t<decltype(argument)>>) {
            return std::to_string(argument);
        } else {
            return std::string(argument);
        }
    };
    std::array<std::string, sizeof...(Args)> elements{pack_one(args)...};
    std::size_t curr = 0;
    for (std::size_t i = 0; i < data.size(); i++) {
        // If we're have '{}' token, insert parameter at this place.
        if (data[i] == '{' && data[i + 1] == '}') {
            formatted += elements[curr++];
        }
        // Add all characters from source string except '{}' token.
        else if (data[i - 1] != '{' || data[i] != '}') {
            formatted += data[i];
        }
    }
    return formatted;
}

#endif //WEAK_FORMAT_HPP
