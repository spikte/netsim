#include "../../include/sim/type.hpp"

std::string bytes2str(const std::vector<std::uint8_t>& bytes, std::int64_t size) {
    std::ostringstream oss;

    if(size == -1)
        size = bytes.size();
    for (std::size_t i = 0; i < size; ++i) {
        if (i > 0)
            oss << ' ';

        oss << std::uppercase
            << std::hex
            << std::setw(2)
            << std::setfill('0')
            << static_cast<unsigned>(bytes[i]);
    }

    return oss.str();
}

// MAC
std::optional<MACAddress> str2mac(std::string address, MACAddressFormat format) {
    switch (format) {
        case MACAddressFormat::Hex:
            return str2mac_hex(address);
        case MACAddressFormat::HexRaw:
            return str2mac_hex_raw(address);
        case MACAddressFormat::Dec:
            return str2mac_dec(address);
    }
}
std::optional<MACAddress> str2mac_hex(std::string address) {
    std::transform(address.begin(), address.end(), address.begin(), ::toupper);
    std::regex pattern(R"((([0-9A-F]{2}):){5}[0-9A-F]{2})");
    if (!std::regex_match(address, pattern))
        return std::nullopt;
    std::uint64_t result;

    result = 0;
    for (std::size_t i = 0; i < 6; i++) {
        std::size_t start   = i * 3;
        std::uint64_t value = std::stoull(address.substr(start, 2), nullptr, 16);
        result |= value << ((5 - i) * 8);
    }

    return result;
}
std::optional<MACAddress> str2mac_hex_raw(std::string address) {
    std::transform(address.begin(), address.end(), address.begin(), ::toupper);
    std::regex pattern(R"([0-9A-F]{12})");
    if (!std::regex_match(address, pattern))
        return std::nullopt;
    std::uint64_t result = 0;

    for (std::size_t i = 0; i < 6; i++) {
        std::uint64_t value = std::stoull(address.substr(i * 2, 2), nullptr, 16);
        result |= value << ((5 - i) * 8);
    }
    return result;
}
std::optional<MACAddress> str2mac_dec(std::string address) {
    std::regex pattern(R"([0-9]+)");
    if (!std::regex_match(address, pattern))
        return std::nullopt;
    try {
        std::uint64_t result = std::stoull(address);
        if (result > 0xFFFFFFFFFFFFULL)
            return std::nullopt;
        return result;
    } catch (const std::exception&) {
        return std::nullopt;
    }
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
// IP
std::optional<IPv4Address> str2ipv4_dec(std::string address) {
    std::regex pattern(R"(([0-9]{1,3}\.){3}[0-9]{1,3})");
    if (!std::regex_match(address, pattern))
        return std::nullopt;
    std::uint64_t result;
    std::size_t start;
    std::size_t end;

    result = 0;
    start  = 0;
    for (std::size_t i = 0; i < 4; i++) {
        end = address.find('.', start);
        if (end == std::string::npos)
            end = address.size();
        std::uint32_t value = std::stoul(address.substr(start, end - start));
        if (value > 255)
            return std::nullopt;
        result |= value << ((3 - i) * 8);
        start = end + 1;
    }

    return result;
}
std::optional<IPv4Address> str2ipv4_dec_raw(std::string address) {
    std::regex pattern(R"([0-9]+)");
    if (!std::regex_match(address, pattern))
        return std::nullopt;
    try {
        std::uint64_t result = std::stoull(address);
        if (result > 0xFFFFFFFFULL)
            return std::nullopt;
        return result;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}
std::optional<IPv4Address> str2ipv4_hex(std::string address) {
    std::transform(address.begin(), address.end(), address.begin(), ::toupper);

    std::regex pattern(R"([0-9A-F]{8})");
    if (!std::regex_match(address, pattern))
        return std::nullopt;
    try {
        std::uint64_t result = std::stoull(address, nullptr, 16);
        if (result > 0xFFFFFFFFULL)
            return std::nullopt;
        return result;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}
std::optional<IPv4Address> str2ipv4(std::string address, IPV4AddressFormat format) {
    switch (format) {
        case IPV4AddressFormat::Dec:
            return str2ipv4_dec(address);
        case IPV4AddressFormat::DecRaw:
            return str2ipv4_dec_raw(address);
        case IPV4AddressFormat::Hex:
            return str2ipv4_hex(address);
    }
}
std::string ipv42str(IPv4Address address) {
    std::stringstream ss;

    ss << ((address >> 24) & 0xff) << "."
       << ((address >> 16) & 0xff) << "."
       << ((address >> 8) & 0xff) << "."
       << (address & 0xff);

    return ss.str();
}
IPv4Address prefix2subnet(std::uint8_t prefix) {
    if (prefix > 32)
        throw std::invalid_argument("invalid prefix length");
    IPv4Address subnet;

    subnet = (std::uint32_t) ~0 << (32 - prefix);

    return subnet;
}
IPv4Address prefix2wildcard(std::uint8_t prefix) {
    if (prefix > 32)
        throw std::invalid_argument("invalid prefix length");
    return ~prefix2subnet(prefix);
}
std::uint8_t subnet2prefix(std::uint32_t subnet) {
    std::uint8_t prefix = 0;

    while (subnet & 0x80000000u) {
        prefix++;
        subnet <<= 1;
    }

    return prefix;
}
IPv4CIDR::IPv4CIDR(IPv4Address address, IPv4Address subnet) : address(address), subnet(subnet) {}
IPv4CIDR::IPv4CIDR(IPv4Address address, std::uint8_t prefix) : address(address) {
    subnet = prefix2subnet(prefix);
}
IPv4CIDR::IPv4CIDR(const std::string& address_str) {
    std::regex pattern(R"(([0-9]{1,3}\.){3}[0-9]{1,3}\/[0-9]{1,2})");
    if (!std::regex_match(address_str, pattern))
        throw std::invalid_argument("invalid ipv4 address");
    std::vector<std::string> address_str_split = split(address_str, "/");
    this->address                              = *str2ipv4_dec(address_str_split[0]);
    std::uint8_t prefix                        = static_cast<std::uint8_t>(std::stoi(address_str_split[1]));
    set_prefix(prefix);
}
void IPv4CIDR::set_address(IPv4Address address) {
    this->address = address;
}
void IPv4CIDR::set_subnet(IPv4Address subnet) {
    this->subnet = subnet;
}
void IPv4CIDR::set_wildcard(IPv4Address wildcard) {
    this->subnet = ~wildcard;
}
void IPv4CIDR::set_prefix(std::uint8_t prefix) {
    this->subnet = prefix2subnet(prefix);
}
IPv4Address IPv4CIDR::get_address() const {
    return address;
}
IPv4Address IPv4CIDR::get_subnet() const {
    return subnet;
}
IPv4Address IPv4CIDR::get_wildcard() const {
    return ~subnet;
}
std::uint8_t IPv4CIDR::get_prefix() const {
    return subnet2prefix(subnet);
}
bool operator==(const IPv4CIDR& left, const IPv4CIDR& right) {
    return left.address == right.address && left.subnet == right.subnet;
}
