#ifndef NETSIM_TYPE_HPP
#define NETSIM_TYPE_HPP

#include <cstdint>
#include <vector>
#include <stdexcept>
#include <optional>
#include <functional>
#include <regex>
#include "utils.hpp"

using DataRaw  = std::vector<std::uint8_t>;

std::string bytes2str(const std::vector<std::uint8_t>& bytes, std::int64_t size = -1);

/* MAC Address */
using MACAddress = std::uint64_t;
enum class MACAddressFormat {
    Hex,
    HexRaw,
    Dec,
};
std::optional<MACAddress> str2mac_hex(std::string address);
std::optional<MACAddress> str2mac_hex_raw(std::string address);
std::optional<MACAddress> str2mac_dec(std::string address);
std::optional<MACAddress> str2mac(std::string address, MACAddressFormat format = MACAddressFormat::Hex);
std::string mac2str(MACAddress mac_address);

/* IPv4 */
using IPv4Address = std::uint32_t;
enum class IPV4AddressFormat {
    Dec,
    DecRaw,
    Hex,
};
std::optional<IPv4Address> str2ipv4_dec(std::string address);
std::optional<IPv4Address> str2ipv4_dec_raw(std::string address);
std::optional<IPv4Address> str2ipv4_hex(std::string address);
std::optional<IPv4Address> str2ipv4(std::string address, IPV4AddressFormat format = IPV4AddressFormat::Dec);
std::string ipv42str(IPv4Address mac_address);
IPv4Address prefix2subnet(std::uint8_t prefix);
IPv4Address prefix2wildcard(std::uint8_t prefix);
std::uint8_t subnet2prefix(IPv4Address subnet);
struct IPv4CIDR {
    IPv4Address address;
    IPv4Address subnet;

    IPv4CIDR(IPv4Address address = 0, IPv4Address subnet = 0);
    IPv4CIDR(IPv4Address address, std::uint8_t prefix);
    IPv4CIDR(const std::string& address_str);
    void set_address(IPv4Address address);
    void set_subnet(IPv4Address subnet);
    void set_wildcard(IPv4Address wildcard);
    void set_prefix(std::uint8_t prefix);
    IPv4Address get_address() const;
    IPv4Address get_subnet() const;
    IPv4Address get_wildcard() const;
    std::uint8_t get_prefix() const;
    friend bool operator==(const IPv4CIDR& left, const IPv4CIDR& right);
};
template<>
struct std::hash<IPv4CIDR> {
    std::size_t operator()(const IPv4CIDR& r) const {
        return std::hash<std::uint64_t>{}((((std::uint64_t)r.address) << 32) | r.subnet);
    }
};

#endif
