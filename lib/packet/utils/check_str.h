#ifndef UTILS_CHECK_STR_H
#define UTILS_CHECK_STR_H

#include <stddef.h>


int check_decimal_char(char c);
int check_decimal_str(char* c, size_t len);

int check_binary_char(char c);
int check_binary_str(char* c, size_t len);

int check_hex_char(char c);
int check_hex_str(char* c, size_t len);


#endif
