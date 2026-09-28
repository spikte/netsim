#ifndef ICMP_PROCESSOR_HPP
#define ICMP_PROCESSOR_HPP

#include "device.hpp"
#include <algorithm>

struct ICMPProcessor {
    static std::uint16_t identifier;
    static void process_packet_icmp(Device& device, std::size_t port, Ethernet& frame, IPv4& packet);
    static void icmp_echo_request(Device& device, IPv4Address address, std::uint16_t identifier, std::uint16_t sequence, std::uint8_t ttl = 64);
    static void icmp_time_exceeded(Device& device, IPv4Address address, IPv4& packet);
};

#endif
