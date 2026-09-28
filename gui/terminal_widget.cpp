#include "terminal_widget.hpp"

std::vector<int> stringTableMaxLen(const StringTable& table) {
    std::vector<int> maxLen(stringTableCol(table), -1);

    for (int j = 0; j < stringTableCol(table); j++) {
        for (int i = 0; i < stringTableRow(table); i++) {
            int strLen = static_cast<int>(stringTableGet(table, i, j).size());
            if (strLen > maxLen[j])
                maxLen[j] = strLen;
        }
    }

    return maxLen;
}
