#include "../../include/utils/str.h"


std::size_t strslenmax(char** strs, std::size_t n) {
    std::size_t lenmax;
    std::size_t len;

    lenmax = 0;
    for(std::size_t i = 0; i < n; i++) {
        len = strlen(strs[i]);
        if(len > lenmax)
            lenmax = len;
    }
    return lenmax;
}
