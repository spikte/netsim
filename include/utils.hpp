#ifndef NETSIM_UITLS_HPP
#define NETSIM_UITLS_HPP

#include <chrono>
#include <cstdint>
#include <algorithm>
#include <string>
#include <optional>
#include <regex>
#include <iomanip>
#include <sstream>
#include "settings.hpp"
#include "type.hpp"

uint64_t get_timestamp_now_ms();
uint64_t get_timestamp_after_ms(uint64_t milliseconds);
uint32_t get_timestamp_now_s();
std::optional<MACAddress> str2mac(std::string address);
std::optional<IPv4Address> str2ipv4(std::string address);
std::string mac2str(MACAddress mac_address);
std::string ipv42str(IPv4Address mac_address);

#endif
