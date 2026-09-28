#include "../../include/utils/check_str.h"


int check_decimal_char(char c) {
    return c >= '0' && c <= '9';
}
int check_decimal_str(char* c, std::size_t len) {
    for(std::size_t i = 0; i < len; i++)
        if(!check_decimal_char(c[i]))
            return 0;
    return 1;
}

int check_binary_char(char c) {
    return c == '0' || c == '1';
}
int check_binary_str(char* c, std::size_t len) {
    for(std::size_t i = 0; i < len; i++)
        if(!check_binary_char(c[i]))
            return 0;
    return 1;
}

int check_hex_char(char c) {
    return (c >= '0' && c <= '9') ||
           (c >= 'A' && c <= 'F');
}
int check_hex_str(char* c, std::size_t len) {
    for(std::size_t i = 0; i < len; i++)
        if(!check_hex_char(c[i]))
            return 0;
    return 1;
}
