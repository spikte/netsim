#include "../include/computer.hpp"


Computer::Computer() : Device(1) {}
void Computer::arp_request(size_t port, MACAddress mac_address) {
    if(!has_interface(port))
        return;
    arpheader_t request;
    arpheader_set_htype(&request, HTYPE_Ethernet_10Mb);
    arpheader_set_ptype(&request, ETHERTYPE_IP);
    arpheader_set_hlen(&request, 6);
    arpheader_set_plen(&request, 4);
    arpheader_set_oper(&request, ARPOP_REQUEST);
    arpheader_set_sha(&request, interfaces[port].address);
    arpheader_set_spa(&request, address);
    arpheader_set_tha(&request, mac_address);
    arpheader_set_tpa(&request, 0);
    PacketRaw packet;
    packet.resize(arpheader_size(&request));
    arpheader_to_bytes(&request, packet.data());
    send_packet(port, packet);
}
void Computer::arp_add_entry(size_t port, IPv4Address ipv4_address, MACAddress mac_address) {
    auto name = interface_name(port);
    if(!name)
        return;
    arp_table.add_entry(ipv4_address, mac_address, *name);
}
void Computer::arp_del_entry(IPv4Address ipv4_address) {
    arp_table.del_entry(ipv4_address);
}
void Computer::process_packet_arp(size_t port, PacketRaw& packet, ethernet_t* ethernet) {
    arpheader_t arp;

    arpheader_from_bytes(packet.data(), &arp);
    if (arp.tpa != address)
        return;
    switch (arp.oper) {
        case ARPOP_REQUEST: {
            arpheader_t reply;
            arpheader_set_htype(&reply, HTYPE_Ethernet_10Mb);
            arpheader_set_ptype(&reply, ETHERTYPE_IP);
            arpheader_set_hlen(&reply, 6);
            arpheader_set_plen(&reply, 4);
            arpheader_set_oper(&reply, ARPOP_REPLY);
            arpheader_set_sha(&reply, interfaces[port].address);
            arpheader_set_spa(&reply, address);
            arpheader_set_tha(&reply, arp.sha);
            arpheader_set_tpa(&reply, arp.spa);
            PacketRaw packet;
            packet.resize(arpheader_size(&reply));
            arpheader_to_bytes(&reply, packet.data());
            send_packet(0, packet);
            break;
        }
        case ARPOP_REPLY: {
            arp_table.add_entry(arp.spa, arp.sha, interfaces[port].name);
            break;
        }
        default:
            break;
    }
}
void Computer::process_packet(size_t port, PacketRaw& packet, ethernet_t* ethernet) {
    switch (ethernet->header.eth_type) {
        case ETHERTYPE_ARP:
            process_packet_arp(port, packet, ethernet);
            break;
        default:
            break;
    }
}
ssize_t Computer::process_packet_one(size_t port) {
    if (!has_interface(port) || interfaces[port].recv_queued_bytes == 0)
        return -1;
    Interface& interface = interfaces[port];
    PacketRaw& packet    = interface.recv_queue.front();
    size_t packet_size   = packet.size();
    ethernet_t ethernet;

    interface.recv_queued_bytes -= packet.size();
    ethernet_from_bytes(&ethernet, packet.data(), packet.size(), false);
    if (ethernet.header.destination == interfaces[port].address) {
    }
    interface.recv_queue.pop_front();

    return packet_size;
}
void Computer::update() {
    if (!on)
        return;
    arp_table.update();
    Device::update();
}
