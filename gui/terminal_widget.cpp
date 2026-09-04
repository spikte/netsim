#include "terminal_widget.hpp"

std::vector<int> stringTableMaxLen(const StringTable& table) {
    std::vector<int> maxLen(stringTableCol(table), -1);

    for (int j = 0; j < stringTableCol(table); j++) {
        for (int i = 0; i < stringTableRow(table); i++) {
            size_t strLen = stringTableGet(table, i, j).size();
            if (strLen > maxLen[j])
                maxLen[j] = strLen;
        }
    }

    return maxLen;
}

CommandTrie<TerminalWidget<Computer>> computer_command_trie;
void build_computer_command_trie() {
    using ArgType      = const std::vector<std::string>&;
    using TerminalType = TerminalWidget<Computer>*;
    computer_command_trie.add("arp -a", [](ArgType args, TerminalType terminal) {
        ARPTable& arp_table = terminal->device->arp_table;
        StringTable string_table;
        string_table.push_back({"Address",
                                "Hardware Addr",
                                "Interface"});
        for (auto const [key, val] : arp_table.table) {
            string_table.push_back({ipv42str(val.ipv4_address),
                                    mac2str(val.mac_address),
                                    val.interface_name});
        }
        terminal->appendTable(string_table);
    });
    computer_command_trie.add("arp -n <arg>", [](ArgType args, TerminalType terminal) {
        if (args.empty())
            return;
        Computer* computer = terminal->device;
        if (auto ipv4_address = str2ipv4(args[0]))
            computer->arp_request(0, *ipv4_address);
    });
    computer_command_trie.add("arp -s <arg> <arg>", [](ArgType args, TerminalType terminal) {});
    computer_command_trie.add("arp -d <arg>", [](ArgType args, TerminalType terminal) {});
}
