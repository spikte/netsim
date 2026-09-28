#include "../../include/sim/routing_processor.hpp"
#include "../../include/sim/arp_processor.hpp"
#include "../../include/sim/icmp_processor.hpp"

void RoutingProcessor::schedule_forward(Device& device, IPv4& packet, std::uint64_t scheduled_tick) {
    ForwardData* route;

    route = device.forwarding_table->resolve(packet.header.destination);
    if (!route)
        return;
    auto it = device.name_to_port.find(route->interface);
    if (it == device.name_to_port.end()) // Should not happen in theory
        return;
    IPv4Address arp_target;
    if (route->next_hop == 0)
        arp_target = packet.header.destination;
    else
        arp_target = route->next_hop;
    std::size_t port_to_forward = it->second;
    auto result                 = device.arp_table->get(arp_target);
    ARPPacketCacheKey key{port_to_forward, arp_target};
    if (result.has_value()) {
        if (result->state == ARPState::Pending) {
            (*device.arp_packet_cache)[key].push_back({packet, scheduled_tick});
        } else {
            if (packet.header.time_to_live <= 1) {
                ICMPProcessor::icmp_time_exceeded(device, packet.header.source, packet);
                return;
            }
            packet.header.set_time_to_live(packet.header.time_to_live - 1);
            packet.header.checksum = packet.compute_header_checksum();
            device.schedule_ipv4(port_to_forward, packet, result->mac_address, scheduled_tick);
        }
    } else {
        (*device.arp_packet_cache)[key].push_back({packet, scheduled_tick});
        ARPProcessor::send_arp_request(device, arp_target, port_to_forward);
    }
}
void RoutingProcessor::forward(Device& device, IPv4& packet) {
    RoutingProcessor::schedule_forward(device, packet, global_tick);
}
std::optional<IPv4> RoutingProcessor::parse_packet(Ethernet& frame) {
    IPv4 packet;
    if (!packet.from_bytes(frame.payload.data(), frame.payload.size()))
        return std::nullopt;
    if (packet.header.checksum != packet.compute_header_checksum())
        return std::nullopt;
    if (packet.header.time_to_live == 0)
        return std::nullopt;
    if (packet.header.total_length != frame.payload.size())
        return std::nullopt;
    return packet;
}
void RoutingProcessor::schedule_ipv4(Device& device, IPv4Address destination, IPv4Protocol protocol, std::vector<uint8_t> data, std::uint64_t scheduled_tick, std::uint8_t ttl) {
    IPv4 packet;
    ForwardData* route;

    // Set ip field
    packet.header.destination = destination;
    packet.header.set_version(4);
    packet.header.set_header_length(5);
    packet.data = data;
    packet.header.set_total_length(20 + packet.data.size());
    packet.header.set_time_to_live(ttl);
    packet.header.set_protocol(static_cast<std::uint8_t>(protocol));
    // Get next hop
    route = device.forwarding_table->resolve(destination);
    if (!route)
        return;
    auto it = device.name_to_port.find(route->interface);
    if (it == device.name_to_port.end()) // Should not happen in theory
        return;
    std::size_t port_to_forward = it->second;
    packet.header.source        = device.interfaces[port_to_forward].ipv4_cidr.address;
    IPv4Address arp_target;
    if (route->next_hop == 0)
        arp_target = packet.header.destination;
    else
        arp_target = route->next_hop;
    auto result = device.arp_table->get(arp_target);
    ARPPacketCacheKey key{port_to_forward, arp_target};
    if (result.has_value()) {
        if (result->state == ARPState::Pending) {
            (*device.arp_packet_cache)[key].push_back({packet, scheduled_tick});
        } else {
            packet.header.checksum = packet.compute_header_checksum();
            device.schedule_ipv4(port_to_forward, packet, result->mac_address, scheduled_tick);
        }
    } else {
        (*device.arp_packet_cache)[key].push_back({packet, scheduled_tick});
        ARPProcessor::send_arp_request(device, arp_target, port_to_forward);
    }
}
void RoutingProcessor::send_ipv4(Device& device, IPv4Address destination, IPv4Protocol protocol, std::vector<uint8_t> data, std::uint8_t ttl) {
    RoutingProcessor::schedule_ipv4(device, destination, protocol, data, global_tick, ttl);
}
