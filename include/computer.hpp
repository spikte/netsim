#ifndef COMPUTER_HPP
#define COMPUTER_HPP

#include "arp_table.hpp"
#include "device.hpp"

struct Computer : Device {
    IPv4Address address;
    ARPTable arp_table;

    Computer();

    void arp_request(size_t port, MACAddress mac_address);
    void arp_add_entry(size_t port, IPv4Address ipv4_address, MACAddress mac_address);
    void arp_del_entry(IPv4Address ipv4_addrses);

    void process_packet_arp(size_t port, PacketRaw& packet, ethernet_t* ethernet);
    void process_packet(size_t port, PacketRaw& packet, ethernet_t* ethernet);
    ssize_t process_packet_one(size_t port) override;
    void update() override;
};

#endif
