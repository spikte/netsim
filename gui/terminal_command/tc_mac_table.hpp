#ifndef GUI_TC_MAC_TABLE_HPP
#define GUI_TC_MAC_TABLE_HPP

#include "../terminal_widget.hpp"

template <typename T>
HandlerReturnType terminal_show_mac_address_table(ArgType args, TerminalType<T> terminal) {
    MACTable& mac_table = *terminal->item->device->mac_table;
    StringTable string_table;
    string_table.push_back({"Vlan", "Mac Address", "Port"});
    for (auto const [key, val] : mac_table.table) {
        string_table.push_back({std::to_string(key.vlan),
                                mac2str(key.address),
                                terminal->item->device->interfaces[val.interface_id].name});
    }
    terminal->appendTable(string_table);
    return HandlerReturnType::Done;
}
template <typename T>
void build_tc_mac_table(CommandTrie<TerminalWidget<T>>& command_trie) {
    command_trie.add("show mac address-table", terminal_show_mac_address_table<T>);
}

#endif
