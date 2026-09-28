#ifndef GUI_TC_IPV4_HPP
#define GUI_TC_IPV4_HPP

#include "../terminal_widget.hpp"

// IPv4
template <typename T>
HandlerReturnType terminal_set_ip_address_single(ArgType args, TerminalType<T> terminal) {
    Device* device = terminal->item->device;
    if (!device->has_interface(0)) {
        terminal->appendMessage("This device doesn't have interface.");
        return HandlerReturnType::Invalid;
    }
    auto ipv4_address = str2ipv4(args[0]);
    if (!ipv4_address) {
        terminal->appendMessage("IP address invalid.");
        return HandlerReturnType::Invalid;
    }
    auto ipv4_wildcard = str2ipv4(args[1]);
    if (!ipv4_wildcard) {
        terminal->appendMessage("IP subnet invalid.");
        return HandlerReturnType::Invalid;
    }
    device->interface_set_ipv4(0, IPv4CIDR(*ipv4_address, ~*ipv4_wildcard));
    return HandlerReturnType::Done;
}
template <typename T>
HandlerReturnType terminal_show_ip_address_single(ArgType args, TerminalType<T> terminal) {
    Device* computer = terminal->item->device;
    if (!computer->has_interface(0)) {
        terminal->appendMessage("This device doesn't have interface.");
        return HandlerReturnType::Invalid;
    }
    std::string message = std::format("IPv4 address: {}\nIPv4 subnet: {}",
                                      ipv42str(computer->interfaces[0].ipv4_cidr.address),
                                      ipv42str(computer->interfaces[0].ipv4_cidr.subnet));
    terminal->appendMessage(message);
    return HandlerReturnType::Done;
}
// IPv4 interface
template <typename T>
HandlerReturnType terminal_show_ip_interface(ArgType args, TerminalType<T> terminal) {
    Device* device = terminal->item->device;
    StringTable ip_table;
    ip_table.push_back({"Interface", "IP Address", "Subnet"});
    for (auto const& interface : device->interfaces)
        ip_table.push_back({interface.name, ipv42str(interface.ipv4_cidr.address), ipv42str(interface.ipv4_cidr.subnet)});
    terminal->appendTable(ip_table);
    return HandlerReturnType::Done;
}
template <typename T>
HandlerReturnType terminal_set_ip_address_interface(ArgType args, TerminalType<T> terminal) {
    Device* device = terminal->item->device;
    auto it        = device->name_to_port.find(args[0]);
    if (it == device->name_to_port.end()) {
        terminal->appendMessage(std::format("Interface {} not found.", args[0]));
        return HandlerReturnType::Invalid;
    }
    auto ipv4_address = str2ipv4(args[1]);
    if (!ipv4_address) {
        terminal->appendMessage(std::format("{} is not a valid ip address.", args[1]));
        return HandlerReturnType::Invalid;
    }
    auto ipv4_wildcard = str2ipv4(args[2]);
    if (!ipv4_wildcard) {
        terminal->appendMessage("IP subnet invalid.");
        return HandlerReturnType::Invalid;
    }
    device->interface_set_ipv4(it->second, IPv4CIDR(*ipv4_address, ~*ipv4_wildcard));
    return HandlerReturnType::Done;
}
template <typename T>
HandlerReturnType terminal_show_ip_route(ArgType args, TerminalType<T> terminal) {
    Device* device = terminal->item->device;
    auto nodes     = device->routing_table->ip_trie.get_nodes();
    for (const auto* node : nodes) {
        for (const auto& route : node->data) {
            std::string last_part;
            if (route.is_direct())
                last_part = std::format("is directly connected, {}", route.interface);
            else
                last_part = std::format("[{}/{}] via {}", route.administrative_distance, route.metric, ipv42str(route.next_hop));
            terminal->appendMessage(std::format("{} {}/{} {}",
                                                rtb2str(route.protocol),
                                                ipv42str(route.ipv4_cidr.address),
                                                std::to_string(route.ipv4_cidr.get_prefix()),
                                                last_part));
        }
    }
    return HandlerReturnType::Done;
}
template <typename T>
HandlerReturnType terminal_set_ip_route(ArgType args, TerminalType<T> terminal) {
    auto destination = str2ipv4(args[0]);
    if (!destination.has_value()) {
        terminal->appendMessage(std::format("{} is not a valid ip address.", args[0]));
        return HandlerReturnType::Invalid;
    }
    auto subnet = str2ipv4(args[1]);
    if (!subnet.has_value()) {
        terminal->appendMessage(std::format("{} is not a valid subnet.", args[1]));
        return HandlerReturnType::Invalid;
    }
    auto nextHop = str2ipv4(args[2]);
    if (!nextHop.has_value()) {
        terminal->appendMessage(std::format("{} is not a valid next hop.", args[2]));
        return HandlerReturnType::Invalid;
    }
    Device* device = terminal->item->device;
    device->routing_table->add_entry(
        IPv4CIDR(*destination, ~(IPv4Address) *subnet),
        *nextHop,
        "",
        0,
        RoutingTableProtocol::Static,
        1);
    device->forwarding_table->build(*device->routing_table);
    return HandlerReturnType::Done;
}
template <typename T>
void build_tc_ip_single(CommandTrie<TerminalWidget<T>>& command_trie) {
    command_trie.add("ip address <arg> <arg>", terminal_set_ip_address_single<T>);
    command_trie.add("ip address", terminal_show_ip_address_single<T>);
    command_trie.add("show ip route", terminal_show_ip_route<T>);
    command_trie.add("ip route <arg> <arg> <arg>", terminal_set_ip_route<T>);
}
template <typename T>
void build_tc_ip_interface(CommandTrie<TerminalWidget<T>>& command_trie) {
    command_trie.add("interface <arg> ip address <arg> <arg>", terminal_set_ip_address_interface<T>);
    command_trie.add("show ip interface", terminal_show_ip_interface<T>);
    command_trie.add("show ip route", terminal_show_ip_route<T>);
    command_trie.add("ip route <arg> <arg> <arg>", terminal_set_ip_route<T>);
}

#endif
