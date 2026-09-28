#include "../../include/sim/stp_processor.hpp"

BPDU STPProcessor::build_root_bpdu_stp(STPDevice& device, std::size_t port) {
    BPDU bpdu;
    bpdu.protocol_id    = 0;
    bpdu.version_id     = 0;
    bpdu.bpdu_type      = static_cast<std::uint8_t>(BPDUType::Config);
    bpdu.flags          = 0;
    bpdu.root_id        = device.root_id;
    bpdu.root_path_cost = device.root_cost;
    bpdu.bridge_id      = device.bridge_id();
    bpdu.port_id        = device.ports[port].port_id();
    bpdu.message_age    = 0;
    bpdu.max_age        = tick_to_second(device.max_age) * 256;
    bpdu.hello_time     = tick_to_second(device.hello_time) * 256;
    bpdu.forward_delay  = tick_to_second(device.forward_delay) * 256;

    return bpdu;
}
BPDU STPProcessor::build_root_bpdu_rstp(STPDevice& device, std::size_t port) {}
BPDU STPProcessor::build_root_bpdu_mstp(STPDevice& device, std::size_t port) {}
BPDU STPProcessor::build_root_bpdu_pvstplus(STPDevice& device, std::size_t port) {}
BPDU STPProcessor::build_root_bpdu_rapid_pvstplus(STPDevice& device, std::size_t port) {}

BPDU forward_bpdu_stp(STPDevice& device, std::size_t port, BPDU bpdu) {
    STPPort stp_port = device.ports[port];
}

void STPProcessor::update_egress(STPDevice& device, std::size_t port) {
    if (!device.device->has_interface(port))
        return;
    if (!device.ports[port].enabled)
        return;
    // Build BPDU
}
void STPProcessor::process_frame_stp(Device& device, std::size_t port, Ethernet& frame) {
    //BPDU bpdu;
    //STPDevice& stp_device = (*device.stp_devices)[0]; // For the moment we don't handle vlan, just use the entry 0 in our STPDevices map

    //if (!bpdu.from_bytes(frame.payload.data(), frame.payload.size()))
    //    return;
    //if (static_cast<STPVersion>(bpdu.version_id) != device.stp_version)
    //    return;
    //STPPort stp_port          = stp_device.ports[port];
    //stp_port.received_root_id = bpdu.root_id;
    //stp_port.received_root_cost   = bpdu.root_path_cost;
    //stp_port.received_bridge_id   = bpdu.bridge_id;
    //stp_port.received_port_id     = bpdu.port_id;
}
