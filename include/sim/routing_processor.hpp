#ifndef ROUTING_PROCESSOR_HPP
#define ROUTING_PROCESSOR_HPP

#include "../packet/packet.hpp"
#include "device.hpp"

struct RoutingProcessor {
    static void schedule_forward(Device& device, IPv4& packet, std::uint64_t scheduled_tick);
    static void forward(Device& device, IPv4& packet);
    static std::optional<IPv4> parse_packet(Ethernet& frame);
    static void schedule_ipv4(Device& device, IPv4Address destination, IPv4Protocol protocol, std::vector<uint8_t> data, std::uint64_t scheduled_tick, std::uint8_t ttl = 64);
    static void send_ipv4(Device& device, IPv4Address destination, IPv4Protocol protocol, std::vector<uint8_t> data, std::uint8_t ttl = 64);
};

#endif
