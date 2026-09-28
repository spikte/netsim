#ifndef UTILS_BASE_CONVERTION_H
#define UTILS_BASE_CONVERTION_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>


unsigned long long get_max_base(std::size_t base_len, int ndigit);
int get_ndigit_base(unsigned long long nbr, int base_len);
void get_basemap(char* base, std::size_t base_len, int basemap[256]);

int itoa_base(unsigned long long in, int size, char* base, std::size_t baselen, char* out);
int itoa_hex(unsigned long long in, int size, char* out);
int itoa_bin(unsigned long long in, int size, char* out);
int itoa_dec(unsigned long long in, int size, char* out);

void itoa_base_print_re(unsigned long long in, char* base, std::size_t baselen);
void itoa_base_print(unsigned long long in, int size, char* base, std::size_t baselen);
void itoa_bin_print(unsigned long long in, int size);

unsigned long long atoi_base(char* in, std::size_t len, char* base, std::size_t baselen);
unsigned long long atoi_dec(char* in, std::size_t len);
unsigned long long atoi_bin(char* in, std::size_t len);
unsigned long long atoi_hex(char* in, std::size_t len);


#endif
