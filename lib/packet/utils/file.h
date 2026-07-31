#ifndef UTILS_FILE_H
#define UTILS_FILE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


uint8_t* read_bytes(char* path, long* size);
void write_bytes(char* path, uint8_t* bytes, size_t n);


#endif
