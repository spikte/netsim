#ifndef GUI_TC_BASIC_HPP
#define GUI_TC_BASIC_HPP

#include "../terminal_widget.hpp"

template <typename T>
HandlerReturnType tc_set_hostname(ArgType args, TerminalType<T> terminal) {
    terminal->item->device->set_name(args[0]);
    terminal->prompt = QString::fromStdString(std::format("{}> ", terminal->item->device->name));
    return HandlerReturnType::Done;
}
template <typename T>
void build_tc_basic(CommandTrie<TerminalWidget<T>>& command_trie) {
    command_trie.add("hostname <arg>", tc_set_hostname<T>);
}

#endif
