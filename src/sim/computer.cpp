#include "../../include/sim/computer.hpp"

Computer::Computer() : Device(1) {
    this->name       = "Computer";
    arp_table        = ARPTable{};
    routing_table    = RoutingTable{};
    forwarding_table = ForwardingTable{};
    reassembly_table = PacketReassemblyTable{};
    arp_packet_cache = ARPPacketCache{};
}
void Computer::process_packet(std::size_t port, Ethernet& frame, IPv4& packet) {
    switch (static_cast<IPv4Protocol>(packet.header.protocol)) {
        case IPv4Protocol::ICMP:
            ICMPProcessor::process_packet_icmp(*this, port, frame, packet);
            break;
        default:
            break;
    }
}
bool Computer::process_frame_one(std::size_t port, Ethernet& frame) {
    switch (static_cast<EthType>(frame.header.eth_type)) {
        case EthType::ARP:
            ARPProcessor::process_frame_arp(*this, port, frame);
            return true;
        default:
            break;
    }
    return false;
}
int64_t Computer::process_frame(std::size_t port, Ethernet& frame) {
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
    return frame.size();
}
std::uint16_t Computer::get_vlan() const {
    if (!has_interface_connected(0))
        return 0;
    if (interfaces[0].egress->dst) {
        if (Switch* sw = dynamic_cast<Switch*>(interfaces[0].egress->dst->device)) {
            std::size_t port = interfaces[0].egress->dst->device_port;
            if ((*sw->ports)[port].type == SwitchPortType::ACCESS)
                return (*sw->ports)[port].access_vlan;
            else
                return (*sw->ports)[port].native_vlan;
        }
    }
    return 0;
}
void Computer::update() {
    if (!on)
        return;

    if (!init_update())
        return;
    reassembly_table->update();
    while (!reassembly_table->to_process.empty() && budget > 0) {
        PacketReassemblyValue& value = reassembly_table->to_process.front();
        IPv4 packet = value.reassemble();
        process_packet(0, value.frame, packet);
        budget -= packet.size();
        reassembly_table->to_process.pop();
    }
    update_interfaces_ingress();
}
