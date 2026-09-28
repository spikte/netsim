#ifndef GUI_BUILD_TC_HPP
#define GUI_BUILD_TC_HPP

#include "../terminal_widget.hpp"
#include "tc_arp.hpp"
#include "tc_basic.hpp"
#include "tc_ipv4.hpp"
#include "tc_mac_table.hpp"
#include "tc_ping.hpp"
#include "tc_vlan.hpp"
#include "tc_traceroute.hpp"
#include "tc_lldp.hpp"

// Switch
template <typename T>
void build_tc_switch(CommandTrie<TerminalWidget<T>>& command_trie) {
    build_tc_basic(command_trie);
    build_tc_mac_table(command_trie);
    build_tc_vlan(command_trie);
    build_tc_lldp(command_trie);
}
// Computer
template <typename T>
void build_tc_computer(CommandTrie<TerminalWidget<T>>& command_trie) {
    build_tc_basic(command_trie);
    build_tc_arp(command_trie);
    build_tc_ip_single(command_trie);
    build_tc_ping(command_trie);
    build_tc_traceroute(command_trie);
}
// Router
template <typename T>
void build_tc_router(CommandTrie<TerminalWidget<T>>& command_trie) {
    build_tc_basic(command_trie);
    build_tc_arp(command_trie);
    build_tc_ip_interface(command_trie);
    build_tc_ping(command_trie);
    build_tc_traceroute(command_trie);
    build_tc_lldp(command_trie);
}

extern CommandTrie<TerminalWidget<ComputerItem>> computer_command_trie;
extern CommandTrie<TerminalWidget<SwitchItem>> switch_command_trie;
extern CommandTrie<TerminalWidget<RouterItem>> router_command_trie;

#endif
