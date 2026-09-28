#ifndef UTILS_PRINT_H
#define UTILS_PRINT_H

#include <stdint.h>
#include <stdio.h>

static inline void mac_print_hex(std::uint64_t mac) {
    printf("%02lX:%02lX:%02lX:%02lX:%02lX:%02lX",
           (mac >> 40) & 0xFF,
           (mac >> 32) & 0xFF,
           (mac >> 24) & 0xFF,
           (mac >> 16) & 0xFF,
           (mac >> 8) & 0xFF,
           mac & 0xFF);
}

#endif
