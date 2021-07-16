#ifndef WEAK_TESTS_FORMAT_HPP
#define WEAK_TESTS_FORMAT_HPP

#include "../format.hpp"

namespace format_detail {

template <typename... Args>
void run(std::string assertion_string, std::string string, Args&&... args)
{
    if (auto f = format(string, std::forward<Args>(args)...); f != assertion_string) {
        throw std::runtime_error("FORMAT error, expected " + assertion_string  + ", got " + f);
    }
}
} // namespace format_detail

void run_format_tests()
{
    format_detail::run("1 2 3", "{} {} {}", 1, 2, 3);
    format_detail::run("arg1: 1, arg2: 2, arg3: 3", "arg1: {}, arg2: {}, arg3: {}", 1,    2,    3   );
    format_detail::run("arg1: 1, arg2: 2, arg3: 3", "arg1: {}, arg2: {}, arg3: {}", 1ULL, 2ULL, 3ULL);
    format_detail::run("arg1: Text, arg2: Text", "arg1: {}, arg2: {}",  "Text", std::string("Text"));
}

#endif //WEAK_TESTS_FORMAT_HPP
