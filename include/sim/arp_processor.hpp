#ifndef ARP_PROCESSOR_HPP
#define ARP_PROCESSOR_HPP

#include "../packet/packet.hpp"
#include "arp_table.hpp"
#include "device.hpp"

struct ARPProcessor {
    static void arp_add_entry(Device& device, std::size_t port, IPv4Address ipv4_address, MACAddress mac_address);
    static void arp_del_entry(Device& device, IPv4Address ipv4_addrses);
    static void schedule_arp_request(Device& device, IPv4Address address, std::size_t port, std::uint64_t scheduled_tick);
    static void send_arp_request(Device& device, IPv4Address address, std::size_t port);
    static void process_frame_arp(Device& device, std::size_t port, Ethernet& ethernet);
};

#endif
