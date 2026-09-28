#include "../../include/sim/switch.hpp"

/* == Switch == */
Switch::Switch(std::size_t nport) : Device(nport) {
    ports      = std::vector<SwitchPort>(nport);
    mac_table  = MACTable{};
    arp_table  = ARPTable{};
    vlan_table = VLANTable{};
    lldp_ports = std::vector<LLDPPort>(nport);
    lldp_set_chassis_id_mac(Device::last_mac_address++);
    name = "Switch";
}
int64_t Switch::process_frame(std::size_t port, Ethernet& frame) {
    // Deduct vlan
    auto vlan = SwitchingProcessor::ingress_vlan(*this, port, frame);
    if (!vlan)
        return 0;
    // Learn source mac address
    SwitchingProcessor::learn_mac(*this, *vlan, frame.header.source, port);
    const bool is_destination = is_frame_destination(frame);
    if (is_destination) {
        auto local_port = get_interface_port(frame.header.destination);
        std::size_t destination_port = local_port.has_value() ? *local_port : port;
        SwitchingProcessor::process_frame_one(*this, destination_port, frame, *vlan);
    }
    if(!is_destination || frame.header.destination == BROADCAST_MAC) {
        // Forward
        std::vector<std::size_t> destinations = SwitchingProcessor::forwarding_ports(*this, port, *vlan, frame.header.destination);
        for (std::size_t to_port : destinations)
            SwitchingProcessor::forward_frame(*this, port, to_port, *vlan, frame);
    }
    return frame.size();
}
void Switch::update() {
    if (!init_update())
        return;
    mac_table->update();
    update_interfaces_ingress();
}
