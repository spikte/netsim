#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "settings.hpp"
#include "utils.hpp"

#include <cstddef>
#include <cstdint>
#include <deque>
#include <string>
#include <vector>
#include <array>


extern "C" {
#include "../lib/packet/packet.h"
}

using MACAddress = uint64_t;
using PacketRaw  = std::vector<uint8_t>;
struct InTransitPacket {
    PacketRaw packet;
    uint64_t delivery_tick;
};
struct Interface;

struct PPLinkUni {
    Interface* src;
    Interface* dst;
    uint64_t bytes_per_tick;
    uint32_t speed;

    PPLinkUni(uint64_t bytes_per_tick = 1024 * 1024);
    bool connect_src(Interface* interface);
    Interface* connect_force_src(Interface* interface);
    bool connect_dst(Interface* interface);
    Interface* connect_force_dst(Interface* interface);
    Interface* disconnect_src();
    Interface* disconnect_dst();
    void update();
};
using Link = std::array<PPLinkUni,2>;

struct Interface {
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

    Interface(MACAddress address = 0, uint32_t mtu = 1500, size_t buffer_size = 1024 * 1024);
    bool can_send(size_t packet_size);
    bool can_recv(size_t packet_size);
    void send_packet(PacketRaw packet);
    void recv_packet(PacketRaw packet);
    void update();
};

#endif
