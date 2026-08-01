#include "../include/interface.hpp"

PPLinkUni::PPLinkUni(uint64_t bytes_per_tick) : bytes_per_tick(bytes_per_tick),
                                                src(nullptr),
                                                dst(nullptr) {}
bool PPLinkUni::connect_src(Interface* interface) {
    if (src != nullptr)
        return false;

    src           = interface;
    interface->to = this;

    return true;
}
Interface* PPLinkUni::connect_force_src(Interface* interface) {
    Interface* previous = src;

    src = interface;
    if (previous != nullptr)
        previous->to = nullptr;
    interface->to = this;

    return previous;
}
bool PPLinkUni::connect_dst(Interface* interface) {
    if (dst != nullptr)
        return false;

    dst             = interface;
    interface->from = this;

    return true;
}
Interface* PPLinkUni::connect_force_dst(Interface* interface) {
    Interface* previous = dst;

    dst = interface;
    if (previous != nullptr)
        previous->from = nullptr;
    interface->from = this;

    return previous;
}
Interface* PPLinkUni::disconnect_src() {
    Interface* src;

    src = this->src;
    if (!src)
        return nullptr;
    src->to   = nullptr;
    this->src = nullptr;

    return src;
}
Interface* PPLinkUni::disconnect_dst() {
    Interface* dst;

    dst = this->dst;
    if (!dst)
        return nullptr;
    dst->from = nullptr;
    this->dst = nullptr;

    return dst;
}

// ==== Interface ==== //
Interface::Interface(MACAddress address,
                     const std::string& name,
                     uint32_t mtu,
                     size_t buffer_size) : address(address),
                                           name(name),
                                           on(true),
                                           mtu(mtu),
                                           to(nullptr),
                                           from(nullptr),
                                           send_queue(),
                                           recv_queue(),
                                           send_queued_bytes(0),
                                           recv_queued_bytes(0),
                                           buffer_size(buffer_size) {}

bool Interface::can_send(size_t packet_size) {
    if (!on || packet_size == 0 || packet_size > mtu || packet_size > (int) buffer_size - (int) send_queued_bytes)
        return false;
    return true;
}
bool Interface::can_recv(size_t packet_size) {
    if (!on || packet_size == 0 || packet_size > mtu || packet_size > (int) buffer_size - (int) recv_queued_bytes)
        return false;
    return true;
}
void Interface::send_packet(PacketRaw packet) {
    size_t packet_size_bytes = packet.size();
    if (!to || !can_send(packet_size_bytes))
        return;
    send_queued_bytes += packet_size_bytes;
    InTransitPacket in_transit_packet;

    in_transit_packet.packet        = std::move(packet);
    in_transit_packet.delivery_tick = global_tick + std::max(1lu, (packet_size_bytes / to->bytes_per_tick));
    send_queue.push_back(std::move(in_transit_packet));
}
void Interface::recv_packet(PacketRaw packet) {
    recv_queued_bytes += packet.size();
    recv_queue.push_back(std::move(packet));
}
void Interface::update() {
    if (!on || to == nullptr)
        return;
    while (!send_queue.empty()) {
        InTransitPacket& packet = send_queue.front();
        if (global_tick < packet.delivery_tick)
            break;
        send_queued_bytes -= packet.packet.size();
        to->dst->recv_packet(std::move(packet.packet));
        send_queue.pop_front();
    }
}
