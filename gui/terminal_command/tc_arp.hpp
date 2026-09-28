#ifndef GUI_TC_ARP_HPP
#define GUI_TC_ARP_HPP

#include "../terminal_widget.hpp"

template <typename T>
HandlerReturnType terminal_show_arp_table(ArgType args, TerminalType<T> terminal) {
    ARPTable& arp_table = *terminal->item->device->arp_table;
    StringTable string_table;
    string_table.push_back({"Address",
                            "Hardware Addr",
                            "Interface"});
    for (auto const [key, val] : arp_table.table) {
        if (val.state == ARPState::Resolved) {
            string_table.push_back({ipv42str(val.ipv4_address),
                                    mac2str(val.mac_address),
                                    val.interface_name});
        }
    }
    terminal->appendTable(string_table);
    return HandlerReturnType::Done;
}
template <typename T>
HandlerReturnType terminal_arp_request(ArgType args, TerminalType<T> terminal) {
    if (args.empty())
        return HandlerReturnType::Invalid;
    Device* device = terminal->item->device;
    if (auto ipv4_address = str2ipv4(args[0]))
        ARPProcessor::send_arp_request(*device, *ipv4_address, 0);
    return HandlerReturnType::Done;
}
template <typename T>
void build_tc_arp(CommandTrie<TerminalWidget<T>>& command_trie) {
    command_trie.add("arp -a", terminal_show_arp_table<T>);
    command_trie.add("arp -n <arg>", terminal_arp_request<T>);
}

#endif
