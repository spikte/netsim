#ifndef UTILS_FILE_H
#define UTILS_FILE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


std::uint8_t* read_bytes(char* path, long* size);
void write_bytes(char* path, std::uint8_t* bytes, std::size_t n);


#endif
