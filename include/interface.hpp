#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "settings.hpp"
#include "utils.hpp"

#include "type.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include <string>
#include <vector>

extern "C" {
#include "../lib/packet/packet.h"
}

struct InTransitPacket {
    PacketRaw packet;
    uint64_t departure_tick;
    uint64_t delivery_tick;
};
struct Interface;
PacketRaw packet_raw_tag(PacketRaw packet, uint16_t vlan);
PacketRaw packet_raw_untag(PacketRaw packet, uint16_t vlan);
uint16_t packet_raw_vlan(PacketRaw packet);
PacketRaw ethernet_to_packet_raw(const ethernet_t* ethernet);
ethernet_t packet_raw_to_ethernet(PacketRaw& packet);

struct PPLinkUni {
    uint64_t bytes_per_tick;
    Interface* src;
    Interface* dst;

    PPLinkUni(uint64_t bytes_per_tick = 1024 * 1024);
    bool connect_src(Interface* interface);
    Interface* connect_force_src(Interface* interface);
    bool connect_dst(Interface* interface);
    Interface* connect_force_dst(Interface* interface);
    Interface* disconnect_src();
    Interface* disconnect_dst();
    void update();
};
using Link = std::array<PPLinkUni, 2>;

struct Interface {
    std::string name;
    MACAddress address;
    bool on;
    uint32_t mtu; // Maybe for the simulation i could just remove that, not sure it's really relevent
    PPLinkUni* to;
    PPLinkUni* from;
    std::deque<InTransitPacket> send_queue;
    std::deque<PacketRaw> recv_queue;
    size_t send_queued_bytes;
    size_t recv_queued_bytes;
    size_t buffer_size;
    std::function<void(InTransitPacket* packet)> on_packet_sent;

    Interface(MACAddress address = 0, uint32_t mtu = 1500, size_t buffer_size = 1024 * 1024);
    bool can_send(size_t packet_size);
    bool can_recv(size_t packet_size);
    void send_packet(PacketRaw packet);
    void recv_packet(PacketRaw packet);
    void update();
};
bool link_interface(Interface& int1, Interface& int2, Link& link);

#endif
