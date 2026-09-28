#ifndef GUI_TC_PING_HPP
#define GUI_TC_PING_HPP

#include "network_view.hpp"
#include "terminal_widget.hpp"
#include <QObject>

struct PingJob : public AsyncJob {
    Q_OBJECT
  public:
    Device* device;
    IPv4Address target;
    static std::uint16_t lastIdentifier;
    std::uint16_t identifier;
    int count;
    int sent;
    int received;
    std::uint64_t startTick;
    std::uint64_t offsetTick;
    TerminalSink* terminal;
    std::size_t signalId;
    std::vector<std::uint64_t> sendTicks;
    PingJob(Device* device, IPv4Address target, int count, TerminalSink* terminal);
    void start();
    void abort() override;

  public slots:
    void onTick() override;

  public:
    void handleReply(IPv4& packet, ICMP& icmp);
};

template <typename T>
HandlerReturnType terminal_ping(ArgType args, TerminalType<T> terminal) {
    Device* device       = terminal->item->device;
    auto address_to_ping = str2ipv4(args[0]);
    if (!address_to_ping.has_value()) {
        terminal->appendMessage(std::format("{} is not a valid ip address.", args[0]));
        return HandlerReturnType::Invalid;
    }
    PingJob* job    = new PingJob(device, *address_to_ping, 5, terminal);
    terminal->state = RunningJobState{.job = job};
    QObject::connect(job, &PingJob::finished, terminal, [terminal, job]() {
        job->device->on_icmp.unsubscribe(job->signalId);
        terminal->state = InputState();
        terminal->appendPrompt();
        job->deleteLater();
    });
    NetworkView* view = terminal->item->getNetworkView();
    if (view) {
        view->connect(view, &NetworkView::onTick, job, &PingJob::onTick);
        job->signalId = device->on_icmp.subscribe([job](IPv4& packet, ICMP& icmp) {
            job->handleReply(packet, icmp);
        });
    }
    job->start();
    return HandlerReturnType::Async;
}

template <typename T>
void build_tc_ping(CommandTrie<TerminalWidget<T>>& command_trie) {
    command_trie.add("ping <arg>", terminal_ping<T>);
}

#endif
