#ifndef UTILS_NUMBERS_H
#define UTILS_NUMBERS_H

#include <stdint.h>
#include <stdlib.h>

/* == PACKING == */
static inline uint16_t pack_u16(const uint8_t* buf) {
    return ((uint16_t) buf[0] << 8) |
           ((uint16_t) buf[1]);
}
static inline uint32_t pack_u32(const uint8_t* buf) {
    return ((uint32_t) buf[0] << 24) |
           ((uint32_t) buf[1] << 16) |
           ((uint32_t) buf[2] << 8) |
           ((uint32_t) buf[3]);
}
static inline uint64_t pack_u48(const uint8_t* buf) {
    return ((uint64_t) buf[0] << 40) |
           ((uint64_t) buf[1] << 32) |
           ((uint64_t) buf[2] << 24) |
           ((uint64_t) buf[3] << 16) |
           ((uint64_t) buf[4] << 8) |
           ((uint64_t) buf[5]);
}
static inline uint64_t pack_u64(const uint8_t* buf) {
    return ((uint64_t) buf[0] << 56) |
           ((uint64_t) buf[1] << 48) |
           ((uint64_t) buf[2] << 40) |
           ((uint64_t) buf[3] << 32) |
           ((uint64_t) buf[4] << 24) |
           ((uint64_t) buf[5] << 16) |
           ((uint64_t) buf[6] << 8) |
           ((uint64_t) buf[7]);
}
static inline uint64_t pack_uN(const uint8_t* buf, size_t n) {
    uint64_t value = 0;

    for (size_t i = 0; i < n; i++)
        value = (value << 8) | buf[i];

    return value;
}

/* == UNPACKING == */
static inline void unpack_u16(uint8_t buf[2], uint16_t val) {
    buf[0] = (uint8_t) (val >> 8);
    buf[1] = (uint8_t) val;
}
static inline void unpack_u32(uint8_t buf[4], uint32_t val) {
    buf[0] = (uint8_t) (val >> 24);
    buf[1] = (uint8_t) (val >> 16);
    buf[2] = (uint8_t) (val >> 8);
    buf[3] = (uint8_t) val;
}
static inline void unpack_u48(uint8_t buf[6], uint64_t val) {
    buf[0] = (uint8_t) (val >> 40);
    buf[1] = (uint8_t) (val >> 32);
    buf[2] = (uint8_t) (val >> 24);
    buf[3] = (uint8_t) (val >> 16);
    buf[4] = (uint8_t) (val >> 8);
    buf[5] = (uint8_t) val;
}
static inline void unpack_u64(uint8_t buf[8], uint64_t val) {
    buf[0] = (uint8_t) (val >> 56);
    buf[1] = (uint8_t) (val >> 48);
    buf[2] = (uint8_t) (val >> 40);
    buf[3] = (uint8_t) (val >> 32);
    buf[4] = (uint8_t) (val >> 24);
    buf[5] = (uint8_t) (val >> 16);
    buf[6] = (uint8_t) (val >> 8);
    buf[7] = (uint8_t) val;
}
static inline void unpack_uN(uint8_t* buf, size_t n, uint64_t val) {
    for (size_t i = 0; i < n; ++i)
        buf[i] = (uint8_t)(val >> (8 * (n - 1 - i)));
}

/* == PACKING WITH OFFSET == */
static inline uint16_t pack_u16_at(const uint8_t* buf, size_t* offset) {
    uint16_t val = pack_u16(buf + *offset);
    *offset += 2;
    return val;
}
static inline uint32_t pack_u32_at(const uint8_t* buf, size_t* offset) {
    uint32_t val = pack_u32(buf + *offset);
    *offset += 4;
    return val;
}
static inline uint64_t pack_u48_at(const uint8_t* buf, size_t* offset) {
    uint64_t val = pack_u48(buf + *offset);
    *offset += 6;
    return val;
}
static inline uint64_t pack_u64_at(const uint8_t* buf, size_t* offset) {
    uint64_t val = pack_u64(buf + *offset);
    *offset += 8;
    return val;
}
static inline uint64_t pack_uN_at(const uint8_t* buf, size_t n, size_t* offset) {
    uint64_t val = pack_uN(buf + *offset, n);
    *offset += n;
    return val;
}

/* == UNPACKING WITH OFFSET == */
static inline void unpack_u16_at(uint8_t* buf, size_t* offset, uint16_t val) {
    unpack_u16(buf + *offset, val);
    *offset += 2;
}
static inline void unpack_u32_at(uint8_t* buf, size_t* offset, uint32_t val) {
    unpack_u32(buf + *offset, val);
    *offset += 4;
}
static inline void unpack_u48_at(uint8_t* buf, size_t* offset, uint64_t val) {
    unpack_u48(buf + *offset, val);
    *offset += 6;
}
static inline void unpack_u64_at(uint8_t* buf, size_t* offset, uint64_t val) {
    unpack_u64(buf + *offset, val);
    *offset += 8;
}
static inline void unpack_uN_at(uint8_t* buf, size_t n, size_t* offset, uint64_t val) {
    unpack_uN(buf + *offset, n, val);
    *offset += n;
}

#endif
