#ifndef NETSIM_UITLS_HPP
#define NETSIM_UITLS_HPP

#include "settings.hpp"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <optional>
#include <regex>
#include <sstream>
#include <string>

std::uint64_t get_timestamp_now_ms();
std::uint64_t get_timestamp_after_ms(std::uint64_t milliseconds);
std::uint32_t get_timestamp_now_s();
std::vector<std::string> split(std::string s, const std::string& delimiter);

constexpr std::uint64_t Tbs(std::uint64_t value) {
    return value * 1'000'000'000'000;
}
constexpr std::uint64_t Gbs(std::uint64_t value) {
    return value * 1'000'000'000;
}
constexpr std::uint64_t Mbs(std::uint64_t value) {
    return value * 1'000'000;
}
constexpr std::uint32_t Meters(std::uint32_t value) {
    return value * 100;
}
constexpr std::uint32_t Km(std::uint32_t value) {
    return value * 1000;
}

#endif
