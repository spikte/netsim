#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "settings.hpp"
#include "utils.hpp"

#include <cstddef>
#include <cstdint>
#include <deque>
#include <string>
#include <vector>

extern "C" {
#include "../lib/packet/packet.h"
}

using MACAddress = uint64_t;
using PacketRaw  = std::vector<uint8_t>;
struct Interface;

struct PPLinkUni {
    uint64_t bandwidth;
    Interface* src;
    Interface* dst;
    std::deque<PacketRaw> send_queue;
    size_t queued_bytes;
    size_t buffer_size;
    BytesBudget bytes_budget;

    PPLinkUni(uint64_t bandwidth = 100'000, size_t buffer_size = 1024 * 1024);
    void set_bandwidth(uint64_t bandwidth);
    bool connect_src(Interface* interface);
    Interface* connect_force_src(Interface* interface);
    bool connect_dst(Interface* interface);
    Interface* connect_force_dst(Interface* interface);
    bool can_send(size_t packet_size);
    void send_packet(PacketRaw packet);
    void update();
};

struct Interface {
    MACAddress address;
    std::string name;
    bool on;
    uint32_t mtu; // Maybe for the simulation i could just remove that, not sure it's really relevent
    PPLinkUni* to;
    PPLinkUni* from;
    std::deque<PacketRaw> send_queue;
    std::deque<PacketRaw> recv_queue;
    size_t send_queued_bytes;
    size_t recv_queued_bytes;
    size_t buffer_size;
    BytesBudget bytes_budget;

    Interface(MACAddress address = 0, const std::string& name = "", uint32_t mtu = 1500, size_t buffer_size = 1024 * 1024);
    bool can_send(size_t packet_size);
    bool can_recv(size_t packet_size);
    void send_packet(PacketRaw packet);
    void recv_packet(PacketRaw packet);
    void recv_queue_pop();
    void send_queue_pop();
    void update();
};

#endif
