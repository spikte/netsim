#ifndef GUI_TC_LLDP_HPP
#define GUI_TC_LLDP_HPP

#include "../terminal_widget.hpp"

template <typename T>
HandlerReturnType terminal_lldp_run(ArgType args, TerminalType<T> terminal) {
    Device* device = terminal->item->device;
    for (std::size_t port = 0; port < device->port_count(); port++) {
        (*device->lldp_ports)[port].enable_egress();
        (*device->lldp_ports)[port].enable_ingress();
    }
    return HandlerReturnType::Done;
}
template <typename T>
HandlerReturnType terminal_no_lldp_run(ArgType args, TerminalType<T> terminal) {
    Device* device = terminal->item->device;
    for (std::size_t port = 0; port < device->port_count(); port++) {
        (*device->lldp_ports)[port].disable_egress();
        (*device->lldp_ports)[port].disable_ingress();
    }
    return HandlerReturnType::Done;
}
template <typename T>
HandlerReturnType terminal_interface_lldp_transmit(ArgType args, TerminalType<T> terminal) {
    Device* device = terminal->item->device;
    auto it        = device->name_to_port.find(args[0]);
    if (it == device->name_to_port.end()) {
        terminal->appendMessage(std::format("Interface {} not found.", args[0]));
        return HandlerReturnType::Invalid;
    }
    (*device->lldp_ports)[it->second].enable_egress();
    return HandlerReturnType::Done;
}
template <typename T>
HandlerReturnType terminal_interface_no_lldp_transmit(ArgType args, TerminalType<T> terminal) {
    Device* device = terminal->item->device;
    auto it        = device->name_to_port.find(args[0]);
    if (it == device->name_to_port.end()) {
        terminal->appendMessage(std::format("Interface {} not found.", args[0]));
        return HandlerReturnType::Invalid;
    }
    (*device->lldp_ports)[it->second].disable_egress();
    return HandlerReturnType::Done;
}
template <typename T>
HandlerReturnType terminal_interface_lldp_receive(ArgType args, TerminalType<T> terminal) {
    Device* device = terminal->item->device;
    auto it        = device->name_to_port.find(args[0]);
    if (it == device->name_to_port.end()) {
        terminal->appendMessage(std::format("Interface {} not found.", args[0]));
        return HandlerReturnType::Invalid;
    }
    (*device->lldp_ports)[it->second].enable_ingress();
    return HandlerReturnType::Done;
}
template <typename T>
HandlerReturnType terminal_interface_no_lldp_receive(ArgType args, TerminalType<T> terminal) {
    Device* device = terminal->item->device;
    auto it        = device->name_to_port.find(args[0]);
    if (it == device->name_to_port.end()) {
        terminal->appendMessage(std::format("Interface {} not found.", args[0]));
        return HandlerReturnType::Invalid;
    }
    (*device->lldp_ports)[it->second].disable_ingress();
    return HandlerReturnType::Done;
}
template <typename T>
HandlerReturnType terminal_show_lldp_neighbors(ArgType args, TerminalType<T> terminal) {
    Device* device = terminal->item->device;
    StringTable string_table;
    string_table.push_back({"Device ID", "Local Intf", "Hold-time", "Port ID"});
    for (std::size_t port = 0; port < device->port_count(); port++) {
        LLDPPort& lldp_port = (*device->lldp_ports)[port];
        if (!lldp_port.neighbor_data.has_value())
            continue;
        LLDPNeighborData neigh_data = *lldp_port.neighbor_data;
        string_table.push_back({
            chassis_id_to_str(neigh_data.chassis_id_type, neigh_data.chassis_id),
            device->interfaces[port].name,
            std::to_string(tick_to_second(neigh_data.ttl)),
            port_id_to_str(neigh_data.port_id_type, neigh_data.port_id),
        });
    }
    terminal->appendTable(string_table);
    return HandlerReturnType::Done;
}

template <typename T>
void build_tc_lldp(CommandTrie<TerminalWidget<T>>& command_trie) {
    command_trie.add("lldp run", terminal_lldp_run<T>);
    command_trie.add("no lldp run", terminal_lldp_run<T>);
    command_trie.add("interface <arg> lldp transmit", terminal_interface_lldp_transmit<T>);
    command_trie.add("interface <arg> no lldp transmit", terminal_interface_no_lldp_transmit<T>);
    command_trie.add("interface <arg> lldp receive", terminal_interface_lldp_receive<T>);
    command_trie.add("interface <arg> no lldp receive", terminal_interface_no_lldp_receive<T>);
    command_trie.add("show lldp neighbors", terminal_show_lldp_neighbors<T>);
}

#endif
