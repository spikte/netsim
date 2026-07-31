#ifndef SOFT_ASSERT_H
#define SOFT_ASSERT_H

#include <cstring>
#include <iostream>
#include <string_view>

namespace soft_assert {

inline bool failed = false;

inline void check(bool condition, std::string_view message = {}, const char* file = __FILE__, int line = __LINE__) {
    if (condition)
        return;

    std::cerr << "Assertion failed: ";
    if (!message.empty())
        std::cerr << "    " << message << '\n';
    std::cerr << "    at: " << file << ':' << line << '\n';
    failed = true;
}
template <typename A, typename B>
void equal(const A& a, const B& b, const char* file = __FILE__, int line = __LINE__) {
    if (a == b)
        return;

    std::cerr << "Assertion failed: values are not equal\n"
              << "    left:  " << a << '\n'
              << "    right: " << b << '\n'
              << "    at:    " << file << ':' << line << '\n';
    failed = true;
}
inline void memory_equal(const void* actual, const void* expected, std::size_t length, const char* file = __FILE__, int line = __LINE__) {
    if (std::memcmp(actual, expected, length) == 0)
        return;

    std::cerr << "Assertion failed: memory regions are not equal\n"
              << "    length:   " << length << '\n'
              << "    at:       " << file << ':' << line << '\n';
    failed = true;
}

} // namespace soft_assert

#define SOFT_ASSERT_CHECK(condition, ...)                  soft_assert::check(condition, ##__VA_ARGS__, __FILE__, __LINE__)
#define SOFT_ASSERT_EQUAL(a, b)                            soft_assert::equal(a, b, __FILE__, __LINE__)
#define SOFT_ASSERT_MEMORY_EQUAL(actual, expected, length) soft_assert::memory_equal(actual, expected, length, __FILE__, __LINE__)

#endif
