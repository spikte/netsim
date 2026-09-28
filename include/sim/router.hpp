#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "device.hpp"
#include "routing_table.hpp"
#include "packet_reassembly.hpp"
#include "arp_processor.hpp"
#include "lldp_processor.hpp"
#include "icmp_processor.hpp"
#include "routing_processor.hpp"
#include <queue>

struct Router : Device {
    Router(std::size_t nport = 8);

    void process_packet(std::size_t port, Ethernet& frame, IPv4& packet);
    bool process_frame_one(std::size_t port, Ethernet& frame);
    int64_t process_frame(std::size_t port, Ethernet& frame) override;
    void update() override;
};

#endif
