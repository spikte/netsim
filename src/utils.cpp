#include "../include/utils.hpp"

uint64_t get_timestamp_now_ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}
uint64_t get_timestamp_after_ms(uint64_t milliseconds) {
    return get_timestamp_now_ms() + milliseconds;
}
uint32_t get_timestamp_now_s() {
    return std::chrono::duration_cast<std::chrono::seconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

std::optional<MACAddress> str2mac(std::string address) {
    std::transform(address.begin(), address.end(), address.begin(), ::toupper);
    std::regex pattern(R"((([0-9A-F]{2}):){5}[0-9A-F]{2})");
    if (!std::regex_match(address, pattern))
        return std::nullopt;
    uint64_t result;

    result = 0;
    for (size_t i = 0; i < 6; i++) {
        size_t start   = i * 3;
        uint64_t value = std::stoull(address.substr(start, 2), nullptr, 16);
        result |= value << ((5 - i) * 8);
    }

    return result;
}
std::optional<IPv4Address> str2ipv4(std::string address) {
    std::regex pattern(R"(([0-9]{1,3}\.){3}[0-9]{1,3})");
    if (!std::regex_match(address, pattern))
        return std::nullopt;
    uint64_t result;
    size_t start;
    size_t end;

    result = 0;
    start  = 0;
    for (size_t i = 0; i < 4; i++) {
        end = address.find('.', start);
        if (end == std::string::npos)
            end = address.size();
        uint32_t value = std::stoul(address.substr(start, end - start));
        if (value > 255)
            return std::nullopt;
        result |= value << ((3 - i) * 8);
        start = end + 1;
    }

    return result;
}
std::string mac2str(MACAddress mac_address) {
    std::stringstream ss;

    ss << std::hex << std::setfill('0')
       << std::setw(2) << ((mac_address >> 40) & 0xff) << ":"
       << std::setw(2) << ((mac_address >> 32) & 0xff) << ":"
       << std::setw(2) << ((mac_address >> 24) & 0xff) << ":"
       << std::setw(2) << ((mac_address >> 16) & 0xff) << ":"
       << std::setw(2) << ((mac_address >> 8) & 0xff) << ":"
       << std::setw(2) << (mac_address & 0xff);

    return ss.str();
}
std::string ipv42str(IPv4Address address) {
    std::stringstream ss;

    ss << ((address >> 24) & 0xff) << "."
       << ((address >> 16) & 0xff) << "."
       << ((address >> 8) & 0xff) << "."
       << (address & 0xff);

    return ss.str();
}
