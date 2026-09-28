#ifndef GUI_TC_VLAN_HPP
#define GUI_TC_VLAN_HPP

#include "../terminal_widget.hpp"

template <typename T>
HandlerReturnType terminal_interface_set_switchport_mode(ArgType args, TerminalType<T> terminal) {
    Device* sw = terminal->item->device;
    auto it    = sw->name_to_port.find(args[0]);
    if (it == sw->name_to_port.end()) {
        terminal->appendMessage(std::format("Interface {} not found.", args[0]));
        return HandlerReturnType::Invalid;
    }
    if (args[1] == "trunk")
        terminal->item->portSetType(it->second, SwitchPortType::TRUNK);
    else if (args[1] == "access")
        terminal->item->portSetType(it->second, SwitchPortType::ACCESS);
    else {
        terminal->appendMessage(std::format("{} is not a valid switchport type.", args[1]));
        return HandlerReturnType::Invalid;
    }
    return HandlerReturnType::Done;
}
template <typename T>
HandlerReturnType terminal_interface_set_access_vlan(ArgType args, TerminalType<T> terminal) {
    Device* sw = terminal->item->device;
    auto it    = sw->name_to_port.find(args[0]);
    if (it == sw->name_to_port.end()) {
        terminal->appendMessage(std::format("Interface {} not found.", args[0]));
        return HandlerReturnType::Invalid;
    }
    if ((*sw->ports)[it->second].type != SwitchPortType::ACCESS) {
        terminal->appendMessage(std::format("Interface {} is not of type access.", args[0]));
        return HandlerReturnType::Invalid;
    }
    try {
        std::uint64_t value = std::stoul(args[1]);
        if (value > std::numeric_limits<std::uint16_t>::max())
            throw std::out_of_range("std::uint16_t overflow");
        std::uint16_t vlan = static_cast<std::uint16_t>(value);
        if (!SwitchingProcessor::has_vlan(*sw, vlan))
            terminal->appendMessage(std::format("VLAN with id {} does not exists. Creating vlan {}", vlan, vlan));
        terminal->item->portAddVlan(it->second, vlan);
    } catch (const std::invalid_argument&) {
        terminal->appendMessage(std::format("{} is not a valid vlan id", args[1]));
        return HandlerReturnType::Invalid;
    } catch (const std::out_of_range&) {
        terminal->appendMessage(std::format("{} is not a valid vlan id", args[1]));
        return HandlerReturnType::Invalid;
    }
    return HandlerReturnType::Done;
}
template <typename T>
HandlerReturnType terminal_interface_set_trunk_vlan(ArgType args, TerminalType<T> terminal) {
    Device* sw = terminal->item->device;
    auto it    = sw->name_to_port.find(args[0]);
    if (it == sw->name_to_port.end()) {
        terminal->appendMessage(std::format("Interface {} not found.", args[0]));
        return HandlerReturnType::Invalid;
    }
    if ((*sw->ports)[it->second].type != SwitchPortType::TRUNK) {
        terminal->appendMessage(std::format("Interface {} is not of type trunk.", args[0]));
        return HandlerReturnType::Invalid;
    }
    try {
        std::vector<std::string> vlans_str = split(args[1], ",");
        std::vector<std::uint16_t> vlans;
        for (auto const& vlan_str : vlans_str) {
            std::uint64_t value = std::stoul(vlan_str);
            if (value > std::numeric_limits<std::uint16_t>::max())
                throw std::out_of_range("std::uint16_t overflow");
            vlans.push_back(static_cast<std::uint16_t>(value));
        }
        for (auto const& vlan : vlans) {
            if (!SwitchingProcessor::has_vlan(*sw, vlan))
                terminal->appendMessage(std::format("VLAN with id {} does not exists. Creating vlan {}", vlan, vlan));
            terminal->item->portAddVlan(it->second, vlan);
        }
    } catch (const std::invalid_argument&) {
        terminal->appendMessage(std::format("{} is not a valid vlan id", args[1]));
        return HandlerReturnType::Invalid;
    } catch (const std::out_of_range&) {
        terminal->appendMessage(std::format("{} is not a valid vlan id", args[1]));
        return HandlerReturnType::Invalid;
    }
    return HandlerReturnType::Done;
}
template <typename T>
HandlerReturnType terminal_show_vlan(ArgType args, TerminalType<T> terminal) {
    Device* device        = terminal->item->device;
    VLANTable& vlan_table = *device->vlan_table;
    StringTable string_table;
    string_table.push_back({"Vlan", "Name", "Status", "Ports"});
    for (auto const [key, val] : vlan_table.table) {
        std::vector<std::size_t> ports_in_vlan = SwitchingProcessor::get_ports_in_vlan(*device, key);
        std::string ports_in_vlan_str          = "";
        for (std::size_t i = 0; i < ports_in_vlan.size(); i++) {
            ports_in_vlan_str += *device->interface_name(ports_in_vlan[i]);
            if (i < ports_in_vlan.size() - 1)
                ports_in_vlan_str += ",";
        }
        string_table.push_back({std::to_string(key),
                                val.name,
                                std::to_string(val.active),
                                ports_in_vlan_str});
    }
    terminal->appendTable(string_table);
    return HandlerReturnType::Done;
}
template <typename T>
HandlerReturnType terminal_add_vlan(ArgType args, TerminalType<T> terminal) {
    Device* device        = terminal->item->device;
    VLANTable& vlan_table = *device->vlan_table;
    try {
        std::uint64_t value = std::stoul(args[0]);
        if (value > std::numeric_limits<std::uint16_t>::max())
            throw std::out_of_range("std::uint16_t overflow");
        std::uint16_t vlan = static_cast<std::uint16_t>(value);
        vlan_table.add_entry(vlan, args[1]);
    } catch (const std::invalid_argument&) {
        terminal->appendMessage(std::format("{} is not a valid vlan id", args[1]));
        return HandlerReturnType::Invalid;
    } catch (const std::out_of_range&) {
        terminal->appendMessage(std::format("{} is not a valid vlan id", args[1]));
        return HandlerReturnType::Invalid;
    }
    return HandlerReturnType::Done;
}
template <typename T>
void build_tc_vlan(CommandTrie<TerminalWidget<T>>& command_trie) {
    command_trie.add("interface <arg> switchport mode <arg>", terminal_interface_set_switchport_mode<T>);
    command_trie.add("interface <arg> access vlan <arg>", terminal_interface_set_access_vlan<T>);
    command_trie.add("interface <arg> trunk allowed vlan <arg>", terminal_interface_set_trunk_vlan<T>);
    command_trie.add("show vlan", terminal_show_vlan<T>);
    command_trie.add("vlan <arg> <arg>", terminal_add_vlan<T>);
}

#endif
