#include "tc_ping.hpp"

#include "tc_ping.hpp"

std::uint16_t PingJob::lastIdentifier = 0;
PingJob::PingJob(Device* device, IPv4Address target, int count, TerminalSink* terminal) : device(device),
                                                                                          target(target),
                                                                                          identifier(lastIdentifier),
                                                                                          count(count),
                                                                                          sent(0),
                                                                                          received(0),
                                                                                          startTick(0),
                                                                                          offsetTick(second_to_tick(1)),
                                                                                          terminal(terminal),
                                                                                          signalId(0) {
    lastIdentifier++;
}

void PingJob::start() {
    startTick = global_tick;
    terminal->appendMessage(std::format("PING {}", ipv42str(target)));
    sendTicks.clear();
}

void PingJob::abort() {
    emit finished();
}

void PingJob::onTick() {
    if (sent >= count)
        return;

    if (startTick + sent * offsetTick < global_tick) {
        ICMPProcessor::icmp_echo_request(*device, target, identifier, sent);
        sendTicks.push_back(global_tick);
        sent++;
    }
}

void PingJob::handleReply(IPv4& packet, ICMP& icmp) {
    if (static_cast<ICMPType>(icmp.header.type) != ICMPType::EchoReply)
        return;
    if (icmp.header.get_rest_word(0) != identifier)
        return;
    std::uint16_t sequence = icmp.header.get_rest_word(1);
    if (sequence >= sendTicks.size() || sequence >= count)
        return;
    std::uint64_t tick_duration = global_tick - sendTicks[sequence];
    terminal->appendMessage(std::format("{} bytes from {}: icmp_seq={} ttl={} time={} ms",
                                        icmp.size(),
                                        ipv42str(packet.header.source),
                                        icmp.header.get_rest_word(1),
                                        packet.header.time_to_live,
                                        tick_to_ms(tick_duration)));
    received++;
    if (received == count)
        abort();
}
