#ifndef GUI_TC_TRACEROUTE_HPP
#define GUI_TC_TRACEROUTE_HPP

#include "network_view.hpp"
#include "terminal_widget.hpp"
#include <QObject>
#include <algorithm>

struct TracerouteReply {
    std::uint64_t duration;
    IPv4 packet;
    ICMP icmp;
    bool has_value = false;
};
struct TracerouteJob : public AsyncJob {
    Q_OBJECT
  public:
    Device* device;
    TerminalSink* terminal;
    IPv4Address target;
    std::uint8_t maxHop;
    std::uint8_t probeSet;
    std::uint8_t targetProbeSet;
    std::uint64_t startTick;
    std::uint64_t offsetTick;
    std::uint64_t timeoutTick;
    std::size_t signalId;
    std::vector<std::uint64_t> sendTicks;
    std::vector<std::vector<TracerouteReply>> replies;
    std::size_t probeDisplayed;
    bool receivedEchoReplyTarget;
    TracerouteJob(Device* device, IPv4Address target, TerminalSink* terminal);
    void start();
    void abort() override;

  public slots:
    void onTick() override;

  public:
    void handleReply(IPv4& packet, ICMP& icmp);
    bool handleReplyICMP(IPv4& packet, ICMP& icmp);
};

template <typename T>
HandlerReturnType terminal_traceroute(ArgType args, TerminalType<T> terminal) {
    Device* device        = terminal->item->device;
    auto address_to_trace = str2ipv4(args[0]);
    if (!address_to_trace.has_value()) {
        terminal->appendMessage(std::format("{} is not a valid ip address.", args[0]));
        return HandlerReturnType::Invalid;
    }
    TracerouteJob* job = new TracerouteJob(device, *address_to_trace, terminal);
    terminal->state    = RunningJobState{.job = job};
    QObject::connect(job, &TracerouteJob::finished, terminal, [terminal, job]() {
        job->device->on_icmp.unsubscribe(job->signalId);
        terminal->state = InputState();
        terminal->appendPrompt();
        job->deleteLater();
    });
    NetworkView* view = terminal->item->getNetworkView();
    if (view) {
        view->connect(view, &NetworkView::onTick, job, &TracerouteJob::onTick);
        job->signalId = job->device->on_icmp.subscribe([job](IPv4& packet, ICMP& icmp) {
            job->handleReply(packet, icmp);
        });
    }
    job->start();
    return HandlerReturnType::Async;
}

template <typename T>
void build_tc_traceroute(CommandTrie<TerminalWidget<T>>& command_trie) {
    command_trie.add("traceroute <arg>", terminal_traceroute<T>);
}

#endif
