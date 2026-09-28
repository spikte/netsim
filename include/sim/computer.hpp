#ifndef COMPUTER_HPP
#define COMPUTER_HPP

#include "arp_processor.hpp"
#include "arp_table.hpp"
#include "device.hpp"
#include "icmp_processor.hpp"
#include "routing_processor.hpp"
#include "switch.hpp"
#include <optional>

struct Computer : Device {
    Computer();
    void process_packet(std::size_t port, Ethernet& frame, IPv4& packet);
    bool process_frame_one(std::size_t port, Ethernet& frame);
    int64_t process_frame(std::size_t port, Ethernet& frame) override;
    std::uint16_t get_vlan() const;
    void update() override;
};

#endif
