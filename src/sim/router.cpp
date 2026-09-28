#include "../../include/sim/router.hpp"

Router::Router(std::size_t nport) : Device(nport) {
    arp_table        = ARPTable{};
    routing_table    = RoutingTable{};
    forwarding_table = ForwardingTable{};
    reassembly_table = PacketReassemblyTable{};
    arp_packet_cache = ARPPacketCache{};
    lldp_ports       = std::vector<LLDPPort>(nport);
    lldp_set_chassis_id_mac(Device::last_mac_address++);
    name = "Router";
}
void Router::process_packet(std::size_t port, Ethernet& frame, IPv4& packet) {
    switch (static_cast<IPv4Protocol>(packet.header.protocol)) {
        case IPv4Protocol::ICMP:
            ICMPProcessor::process_packet_icmp(*this, port, frame, packet);
            break;
        default:
            break;
    }
}
bool Router::process_frame_one(std::size_t port, Ethernet& frame) {
    switch (static_cast<EthType>(frame.header.eth_type)) {
        case EthType::ARP:
            ARPProcessor::process_frame_arp(*this, port, frame);
            return true;
        case EthType::LLDP:
            LLDPProcessor::process_frame_lldp(*this, port, frame);
            break;
        default:
            break;
    }
    return false;
}
int64_t Router::process_frame(std::size_t port, Ethernet& frame) {
    if (!is_frame_destination(port, frame))
        return -1;
    if (process_frame_one(port, frame))
        return frame.size();
    auto packet = RoutingProcessor::parse_packet(frame);
    if (!packet.has_value())
        return -1;
    auto destination_port = get_interface_port(packet->header.destination);
    if (destination_port.has_value())
        process_packet(*destination_port, frame, *packet);
    else
        RoutingProcessor::forward(*this, *packet);
    return frame.size();
}
void Router::update() {
    if (!on)
        return;

    if (!init_update())
        return;
    reassembly_table->update();
    while (!reassembly_table->to_process.empty() && budget > 0) {
        PacketReassemblyValue& value = reassembly_table->to_process.front();
        IPv4 packet                  = value.reassemble();
        process_packet(0, value.frame, packet);
        budget -= packet.size();
        reassembly_table->to_process.pop();
    }
    update_interfaces_ingress();
}
