#include "../../include/sim/arp_processor.hpp"
#include "../../include/sim/routing_processor.hpp"

void ARPProcessor::arp_add_entry(Device& device, std::size_t port, IPv4Address ipv4_address, MACAddress mac_address) {
    auto name = device.interface_name(port);
    if (!name)
        return;
    device.arp_table->add_entry(ipv4_address, mac_address, *name);
}
void ARPProcessor::arp_del_entry(Device& device, IPv4Address ipv4_address) {
    device.arp_table->del_entry(ipv4_address);
}
void ARPProcessor::schedule_arp_request(Device& device, IPv4Address address, std::size_t port, std::uint64_t scheduled_tick) {
    if (!device.has_interface(port))
        return;
    ARPHeader request;
    request.set_htype(ARPHardwareType::Ethernet_10Mb);
    request.set_ptype(static_cast<std::uint16_t>(EthType::IP));
    request.set_hlen(6);
    request.set_plen(4);
    request.set_oper(ARPOperation::REQUEST);
    request.set_sha(device.interfaces[port].mac_address);
    request.set_spa(device.interfaces[port].ipv4_cidr.address);
    request.set_tha(0);
    request.set_tpa(address);
    DataRaw data;
    data.resize(request.size());
    request.to_bytes(data.data());
    device.schedule_ethernet_broadcast(port, std::nullopt, EthType::ARP, data, scheduled_tick);
    device.arp_table->add_entry(address);
}
void ARPProcessor::send_arp_request(Device& device, IPv4Address address, std::size_t port) {
    schedule_arp_request(device, address, port, global_tick);
}
void ARPProcessor::process_frame_arp(Device& device, std::size_t port, Ethernet& ethernet) {
    ARPHeader arp;

    arp.from_bytes(ethernet.payload.data());
    auto destination_port = device.get_interface_port(static_cast<IPv4Address>(arp.tpa));
    if (!destination_port.has_value())
        return;
    switch (static_cast<ARPOperation>(arp.oper)) {
        case ARPOperation::REQUEST: {
            ARPHeader reply;
            reply.set_htype(ARPHardwareType::Ethernet_10Mb);
            reply.set_ptype(static_cast<std::uint16_t>(EthType::IP));
            reply.set_hlen(6);
            reply.set_plen(4);
            reply.set_oper(ARPOperation::REPLY);
            reply.set_sha(device.interfaces[*destination_port].mac_address);
            reply.set_spa(device.interfaces[*destination_port].ipv4_cidr.address);
            reply.set_tha(arp.sha);
            reply.set_tpa(arp.spa);
            DataRaw data;
            data.resize(reply.size());
            reply.to_bytes(data.data());
            device.send_ethernet(port, arp.sha, std::nullopt, EthType::ARP, data);
            break;
        }
        case ARPOperation::REPLY: {
            if (arp.tpa != device.interfaces[port].ipv4_cidr.address)
                return;
            device.arp_table->add_entry(arp.spa, arp.sha, device.interfaces[port].name);
            ARPPacketCacheKey key{port, static_cast<IPv4Address>(arp.spa)};
            auto it = device.arp_packet_cache->find(key);
            if (it == device.arp_packet_cache->end())
                break;
            auto scheduled_packets = std::move(it->second);
            device.arp_packet_cache->erase(it);
            for (auto& packet : scheduled_packets)
                RoutingProcessor::schedule_forward(device, packet.packet, packet.scheduled_tick);
            break;
        }
        default:
            break;
    }
}
