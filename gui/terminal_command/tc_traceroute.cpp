#include "tc_traceroute.hpp"

TracerouteJob::TracerouteJob(Device* device, IPv4Address target, TerminalSink* terminal) : device(device),
                                                                                           terminal(terminal),
                                                                                           target(target),
                                                                                           maxHop(64),
                                                                                           probeSet(0),
                                                                                           probeDisplayed(0),
                                                                                           replies({}),
                                                                                           receivedEchoReplyTarget(false),
                                                                                           offsetTick(second_to_tick(1)),
                                                                                           timeoutTick(100),
                                                                                           targetProbeSet(0) {}
void TracerouteJob::start() {
    startTick = global_tick;
    probeSet  = 0;
}
void TracerouteJob::abort() {
    emit finished();
}
void TracerouteJob::onTick() {
    if (probeSet >= maxHop)
        return;
    const std::uint64_t probeTick = startTick + probeSet * offsetTick;
    if (probeTick > global_tick)
        return;
    ICMPProcessor::icmp_echo_request(*device, target, 1234 + probeSet, 0, probeSet + 1);
    ICMPProcessor::icmp_echo_request(*device, target, 1234 + probeSet, 1, probeSet + 1);
    ICMPProcessor::icmp_echo_request(*device, target, 1234 + probeSet, 2, probeSet + 1);
    sendTicks.push_back(global_tick);
    probeSet++;
}

void TracerouteJob::handleReply(IPv4& packet, ICMP& icmp) {
    if (!handleReplyICMP(packet, icmp))
        return;

    for (std::size_t i = probeDisplayed; i < probeSet && i < replies.size(); i++) {
        const bool all_checked = replies[i].size() == 3 &&
                                 std::all_of(replies[i].begin(), replies[i].end(), [](const TracerouteReply& reply) {
                                     return reply.has_value;
                                 });
        const bool timeout     = !all_checked && global_tick >= sendTicks[i] + timeoutTick;
        if (!all_checked && !timeout)
            break;
        std::string lineStr = std::format(" {} ", i + 1);
        for (std::size_t j = 0; j < 3; j++) {
            if (!replies[i][j].has_value) {
                lineStr += "* ";
                continue;
            }
            if (j != 0 && replies[i][j - 1].has_value && replies[i][j].packet.header.source == replies[i][j - 1].packet.header.source)
                lineStr += std::format("{} ms ", replies[i][j].duration);
            else
                lineStr += std::format("{} {} ms ", ipv42str(replies[i][j].packet.header.source), replies[i][j].duration);
        }
        terminal->appendMessage(lineStr);

        if (receivedEchoReplyTarget && i == targetProbeSet) {
            emit finished();
            return;
        }
        probeDisplayed++;
    }
}

bool TracerouteJob::handleReplyICMP(IPv4& packet, ICMP& icmp) {
    const ICMPType type = static_cast<ICMPType>(icmp.header.type);
    if (type != ICMPType::EchoReply && type != ICMPType::TimeExceeded)
        return false;
    if (type == ICMPType::EchoReply && packet.header.source != target)
        return false;
    std::uint16_t identifier = icmp.header.get_rest_word(0);
    if (identifier < 1234 || identifier >= 1234 + probeSet)
        return false;
    const std::uint16_t sequence = icmp.header.get_rest_word(1);
    if (sequence > 2)
        return false;
    const std::size_t currentProbeSet = identifier - 1234;
    if (currentProbeSet >= sendTicks.size())
        return false;
    if (currentProbeSet < probeDisplayed)
        return false;

    const std::uint64_t tickDuration = global_tick - sendTicks[currentProbeSet];
    if (replies.size() <= currentProbeSet + 1)
        replies.resize(currentProbeSet + 1);
    for (auto& row : replies)
        if (row.size() < 3)
            row.resize(3);
    if (replies[currentProbeSet][sequence].has_value)
        return false;
    replies[currentProbeSet][sequence] = TracerouteReply{.duration  = tickDuration,
                                                         .packet    = packet,
                                                         .icmp      = icmp,
                                                         .has_value = true};
    if (type == ICMPType::EchoReply) {
        receivedEchoReplyTarget = true;
        targetProbeSet          = currentProbeSet;
    }

    return true;
}
