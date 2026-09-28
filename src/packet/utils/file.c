#include "../../include/utils/file.h"


// Source: https://stackoverflow.com/questions/22059189/read-a-file-as-byte-array
std::uint8_t* read_bytes(char* path, long* size) {
    FILE *fileptr;
    std::uint8_t *buffer;
    long filelen;

    fileptr = fopen(path, "rb");
    if(!fileptr)
        return NULL;
    fseek(fileptr, 0, SEEK_END);
    filelen = ftell(fileptr);
    rewind(fileptr);

    buffer = (std::uint8_t *)malloc(filelen * sizeof(std::uint8_t));
    if(!buffer)
        return NULL;
    if((long)fread(buffer, 1, filelen, fileptr) != filelen)
        return NULL;
    fclose(fileptr);
    *size = filelen;
    return buffer;
}
void write_bytes(char* path, std::uint8_t* buffer, std::size_t n) {
    FILE *fileptr;

    fileptr = fopen(path, "wb");
    if(!fileptr)
        return;
    fwrite(buffer, 1, n, fileptr);
    fclose(fileptr);
}
