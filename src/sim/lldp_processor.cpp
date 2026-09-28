#include "../../include/sim/lldp_processor.hpp"

void LLDPProcessor::update_egress(Device& device, std::size_t port) {
    if (!device.has_interface_connected(port))
        return;
    Interface& interface = device.interfaces[port];
    LLDPPort& lldp_port = (*device.lldp_ports)[port];
    if(lldp_port.neighbor_data.has_value() && lldp_port.neighbor_data->last_received_tick + lldp_port.neighbor_data->ttl < global_tick)
        lldp_port.neighbor_data = std::nullopt;
    if (!lldp_port.egress_enabled)
        return;
    if (lldp_port.last_send_tick + lldp_port.offset_tick <= global_tick) {
        lldp_port.last_send_tick = global_tick;
        LLDP lldp;
        lldp.add_chassis_id(*device.chassis_id_type, *device.chassis_id);
        lldp.add_port_id(lldp_port.port_id_type, lldp_port.port_id);
        lldp.add_ttl(lldp_port.ttl);
        device.send_ethernet(port, LLDP_MULTICAST, std::nullopt, EthType::LLDP, std::move(lldp.to_bytes()));
    }
}
void LLDPProcessor::process_frame_lldp(Device& device, std::size_t port, Ethernet& frame) {
    LLDPPort& lldp_port = (*device.lldp_ports)[port];
    if (!lldp_port.ingress_enabled)
        return;
    if (!lldp_port.neighbor_data.has_value())
        lldp_port.neighbor_data = LLDPNeighborData();
    lldp_port.neighbor_data->last_received_tick = global_tick;
    LLDP lldp;

    lldp.from_bytes(frame.payload.data(), frame.payload.size());
    for (auto const& tlv : lldp.tlvs) {
        switch (static_cast<LLDP::TLVType>(tlv.get_type())) {
            case LLDP::TLVType::ChassisId:
                lldp_port.neighbor_data->chassis_id_type = static_cast<LLDP::ChassisIdSubtype>(tlv.data[0]);
                lldp_port.neighbor_data->chassis_id      = std::vector<std::uint8_t>(tlv.data.begin() + 1, tlv.data.end());
                break;
            case LLDP::TLVType::PortId:
                lldp_port.neighbor_data->port_id_type = static_cast<LLDP::PortIdSubtype>(tlv.data[0]);
                lldp_port.neighbor_data->port_id      = std::vector<std::uint8_t>(tlv.data.begin() + 1, tlv.data.end());
                break;
            case LLDP::TLVType::TimeToLive:
                lldp_port.neighbor_data->ttl = pack_u16(tlv.data.data());
                break;
            case LLDP::TLVType::EndOfLLDPDU:
                return;
            default:
                break;
        }
    }
}
