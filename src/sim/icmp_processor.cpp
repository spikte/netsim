#include "../../include/sim/icmp_processor.hpp"
#include "../../include/sim/routing_processor.hpp"

uint16_t ICMPProcessor::identifier = 0;
void ICMPProcessor::process_packet_icmp(Device& device, std::size_t port, Ethernet& frame, IPv4& packet) {
    ICMP icmp;

    if (!icmp.from_bytes(packet.data.data(), packet.data.size()))
        return;
    device.on_icmp.fire(packet, icmp);
    switch (static_cast<ICMPType>(icmp.header.type)) {
        case ICMPType::EchoRequest: {
            if (icmp.header.code != 0)
                return;
            std::vector<uint8_t> bytes = icmp.to_bytes();
            icmp.header.set_type(ICMPType::EchoReply);
            icmp.header.checksum = icmp.compute_header_checksum();
            RoutingProcessor::send_ipv4(device, packet.header.source, IPv4Protocol::ICMP, icmp.to_bytes());
            break;
        }
        case ICMPType::EchoReply:
            break;
        default:
            break;
    }
}
void ICMPProcessor::icmp_echo_request(Device& device, IPv4Address address, std::uint16_t identifier, std::uint16_t sequence, std::uint8_t ttl) {
    ICMP icmp;

    icmp.init();
    icmp.header.type = static_cast<std::uint8_t>(ICMPType::EchoRequest);
    icmp.header.code = static_cast<std::uint8_t>(ICMPCode::NoCode);
    icmp.header.set_rest_word(identifier, 0);
    icmp.header.set_rest_word(sequence, 1);
    icmp.header.checksum = icmp.compute_header_checksum();
    icmp.data            = {0, 1, 2, 3, 4, 5, 6, 7};
    RoutingProcessor::send_ipv4(device, address, IPv4Protocol::ICMP, icmp.to_bytes(), ttl);
}
void ICMPProcessor::icmp_time_exceeded(Device& device, IPv4Address address, IPv4& packet) {
    ICMP icmp;

    icmp.init();
    icmp.header.type                       = static_cast<std::uint8_t>(ICMPType::TimeExceeded);
    icmp.header.code                       = static_cast<std::uint8_t>(ICMPCode::NoCode);
    icmp.header.checksum                   = icmp.compute_header_checksum();
    std::vector<std::uint8_t> packet_bytes = packet.to_bytes();
    const std::size_t payload_to_copy_size = std::min(packet_bytes.size(), (std::size_t) packet.header.get_header_length() + 8);
    icmp.data                              = std::vector<std::uint8_t>(packet_bytes.begin(), packet_bytes.begin() + payload_to_copy_size);
    RoutingProcessor::send_ipv4(device, address, IPv4Protocol::ICMP, icmp.to_bytes());
}
