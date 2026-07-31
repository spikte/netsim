#include "../include/interface.hpp"

PPLinkUni::PPLinkUni(uint64_t bandwidth, size_t buffer_size) : bandwidth(bandwidth),
                                                               src(nullptr),
                                                               dst(nullptr),
                                                               send_queue(),
                                                               queued_bytes(0),
                                                               buffer_size(buffer_size),
                                                               bytes_budget(0) {
    bytes_budget.set_bpt_from_bps(bandwidth);
}
void PPLinkUni::set_bandwidth(uint64_t bandwidth) {
    this->bandwidth = bandwidth;
    bytes_budget.set_bpt_from_bps(bandwidth);
}
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
bool PPLinkUni::can_send(size_t packet_size) {
    if (packet_size == 0)
        return false;
    if (packet_size > buffer_size - queued_bytes)
        return false;
    return true;
}
void PPLinkUni::send_packet(PacketRaw packet) {
    queued_bytes += packet.size();
    send_queue.push_back(std::move(packet));
}
void PPLinkUni::update() {
    if (src == nullptr || dst == nullptr || !src->on)
        return;

    bytes_budget.update();
    while (!send_queue.empty()) {
        PacketRaw& packet = send_queue.front();
        if (!bytes_budget.consume(packet.size()))
            break;
        queued_bytes -= packet.size();
        dst->recv_packet(std::move(packet));
        send_queue.pop_front();
    }
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
                                           buffer_size(buffer_size),
                                           bytes_budget(10e8) {}

bool Interface::can_send(size_t packet_size) {
    if (!on || packet_size == 0 || packet_size > mtu || packet_size > buffer_size - send_queued_bytes)
        return false;
    return true;
}
bool Interface::can_recv(size_t packet_size) {
    if (!on || packet_size == 0 || packet_size > mtu || packet_size > buffer_size - recv_queued_bytes)
        return false;
    return true;
}
void Interface::send_packet(PacketRaw packet) {
    send_queued_bytes += packet.size();
    send_queue.push_back(std::move(packet));
}
void Interface::recv_packet(PacketRaw packet) {
    recv_queued_bytes += packet.size();
    recv_queue.push_back(std::move(packet));
}
void Interface::recv_queue_pop() {
    recv_queued_bytes -= recv_queue.front().size();
    recv_queue.pop_front();
}
void Interface::send_queue_pop() {
    send_queued_bytes -= send_queue.front().size();
    send_queue.pop_front();
}
void Interface::update() {
    if (!on || to == nullptr)
        return;
    bytes_budget.update();
    while (!send_queue.empty()) {
        PacketRaw& packet = send_queue.front();
        if (!bytes_budget.consume(packet.size()) || !to->can_send(packet.size()))
            break;
        send_queued_bytes -= send_queue.front().size();
        to->send_packet(std::move(packet));
        send_queue.pop_front();
    }
}
