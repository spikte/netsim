#include "../../include/sim/interface.hpp"
#include "../../include/sim/device.hpp"

std::uint64_t ethernet_phy_speed(EthernetPhy phy) {
    switch (phy) {
        case EthernetPhy::_10BASE5:
        case EthernetPhy::_10BASE2:
        case EthernetPhy::_10BASE_T:
        case EthernetPhy::_10BASE_FL:
            return Mbs(10);
        case EthernetPhy::_100BASE_TX:
        case EthernetPhy::_100BASE_FX:
            return Mbs(100);
        case EthernetPhy::_1000BASE_T:
        case EthernetPhy::_1000BASE_SX:
        case EthernetPhy::_1000BASE_LX:
            return Gbs(1);
        case EthernetPhy::_10GBASE_T:
        case EthernetPhy::_10GBASE_SR:
        case EthernetPhy::_10GBASE_LR:
            return Gbs(10);
    }
    throw std::invalid_argument("Unknown Ethernet PHY");
}
Interface::Interface(MACAddress address, EthernetPhy phy, PhysicalPortType physical_port_type) {
    name              = "";
    description       = "";
    device            = nullptr;
    device_port       = 0;
    mac_address       = address;
    ipv4_cidr         = IPv4CIDR{0, (IPv4Address) 0};
    on                = true;
    egress            = nullptr;
    ingress           = nullptr;
    send_queued_bytes = 0;
    recv_queued_bytes = 0;
    buffer_size       = 1024 * 1024;
    data_sequence     = 0;
    type              = InterfaceType::Ethernet;
    mtu               = 1500;
    ethernet_config   = EthernetConfig{
        .physical_port_type = physical_port_type,
        .phy                = phy,
    };
    bits_per_second = Gbs(1); // A default, TODO: Change this
}

bool Interface::can_send(std::size_t data_size) {
    if (!on || data_size == 0 || data_size > mtu)
        return false;
    if (send_queued_bytes > buffer_size)
        return false;
    return data_size <= buffer_size - send_queued_bytes;
}
bool Interface::can_recv(std::size_t data_size) {
    if (!on || data_size == 0 || data_size > mtu)
        return false;
    if (recv_queued_bytes > buffer_size)
        return false;
    return data_size <= buffer_size - recv_queued_bytes;
}
bool Interface::send_data(DataRaw data) {
    std::uint64_t last_data_delivery_tick;
    InTransitData in_transit_data;

    std::uint64_t bits_per_second          = std::min({bits_per_second, egress->dst->bits_per_second});
    const double transmission_seconds      = (data.size() * 8.0) / bits_per_second;
    const std::uint64_t transmission_ticks = static_cast<std::uint64_t>(std::ceil(second_to_tick(transmission_seconds)));
    std::uint64_t propagation_speed_tick   = 0;
    if (egress->propagation_speed != 0) {
        propagation_speed_tick = egress->length / egress->propagation_speed; // In seconds
        propagation_speed_tick = second_to_tick(propagation_speed_tick);
    }

    last_data_delivery_tick        = send_queue.empty() ? 0 : send_queue.back().delivery_tick;
    in_transit_data.data           = std::move(data);
    in_transit_data.departure_tick = std::max(global_tick, last_data_delivery_tick);
    in_transit_data.delivery_tick  = in_transit_data.departure_tick + std::max(1lu, transmission_ticks) + propagation_speed_tick;
    send_queue.push_back(std::move(in_transit_data));
    on_data_sent.fire(&send_queue.back());
    return true;
}
bool Interface::schedule(DataRaw data, std::uint64_t scheduled_tick) {
    std::size_t data_size_bytes = data.size();
    if (!can_send(data_size_bytes))
        return false;
    send_queued_bytes += data.size();
    ScheduledData scheduled_data{
        .data           = std::move(data),
        .scheduled_tick = scheduled_tick,
        .sequence       = data_sequence,
    };
    scheduled_queue.push(scheduled_data);
    data_sequence++;
    return true;
}
void Interface::recv_data(DataRaw data) {
    recv_queued_bytes += data.size();
    recv_queue.push_back(std::move(data));
}
void Interface::update() {
    if (!on || !egress)
        return;
    while (!scheduled_queue.empty() && scheduled_queue.top().scheduled_tick <= global_tick) {
        auto packet = std::move(scheduled_queue.top());
        scheduled_queue.pop();
        send_data(std::move(packet.data));
    }
    while (!send_queue.empty()) {
        InTransitData& data = send_queue.front();
        if (!egress->dst) { // The outgoing interface for the data doesn't exist
            send_queue.pop_front();
            continue;
        }
        if (global_tick < data.delivery_tick || !egress->dst->can_recv(data.data.size()))
            break;
        send_queued_bytes -= data.data.size();
        egress->dst->recv_data(std::move(data.data));
        send_queue.pop_front();
    }
}
