#include "../../../include/packet/utils/base_convertion.h"

static unsigned long long int_pow(int n, int p) {
    unsigned long long result;

    result = 1;
    for (int i = 0; i < p; i++)
        result *= (unsigned long long) n;
    return result;
}
unsigned long long get_max_base(std::size_t base_len, int ndigit) {
    unsigned long long max;

    max = 0;
    for (int i = 0; i < ndigit; i++)
        max += int_pow(base_len, i);
    return max;
}
int get_ndigit_base(unsigned long long nbr, int base_len) {
    int ndigit;

    ndigit = 1;
    while (nbr >= (unsigned long long) base_len) {
        nbr /= (unsigned long long) base_len;
        ndigit++;
    }
    return (ndigit);
}
void get_basemap(char* base, std::size_t base_len, int basemap[256]) {
    for (std::size_t i = 0; i < 256; i++)
        basemap[i] = -1;
    for (std::size_t i = 0; i < base_len; i++) {
        basemap[(std::size_t) base[i]] = i;
    }
}

int itoa_base(unsigned long long in, int size, char* base, std::size_t baselen, char* out) {
    int ndigit;
    std::size_t trailing_zero;
    int i;
    std::size_t len;

    ndigit = get_ndigit_base(in, baselen);
    if (size < ndigit)
        size = ndigit;
    trailing_zero = size == -1 ? 0 : size - ndigit;
    len           = trailing_zero + ndigit + 1;
    if (!out)
        return -1;
    out[len - 1] = '\0';
    i            = len - 2;
    while (in != 0) {
        out[i--] = base[in % (unsigned long long) baselen];
        in /= baselen;
    }
    while (i >= 0)
        out[i--] = base[0];
    out[ndigit + trailing_zero] = '\0';
    return len - 1;
}
int itoa_hex(unsigned long long in, int size, char* out) {
    return itoa_base(in, size, "0123456789ABCDEF", 16, out);
}
int itoa_bin(unsigned long long in, int size, char* out) {
    return itoa_base(in, size, "01", 2, out);
}
int itoa_dec(unsigned long long in, int size, char* out) {
    return itoa_base(in, size, "0123456789", 10, out);
}

void itoa_base_print_re(unsigned long long in, char* base, std::size_t baselen) {
    if (in < baselen) {
        printf("%c", base[in]);
        return;
    }
    itoa_base_print_re(in / baselen, base, baselen);
    itoa_base_print_re(in % baselen, base, baselen);
}
void itoa_base_print(unsigned long long in, int size, char* base, std::size_t baselen) {
    int ndigit;
    int trailing_zero;

    ndigit = get_ndigit_base(in, baselen);
    if (size < ndigit)
        size = ndigit;
    trailing_zero = size == -1 ? 0 : size - ndigit;
    for (int i = 0; i < trailing_zero; i++)
        printf("%c", base[0]);
    itoa_base_print_re(in, base, baselen);
}
void itoa_bin_print(unsigned long long in, int size) {
    itoa_base_print(in, size, "01", 2);
}

unsigned long long atoi_base(char* in, std::size_t len, char* base, std::size_t baselen) {
    unsigned long long result;
    int basemap[256];
    unsigned long long mult;

    result = 0;
    get_basemap(base, baselen, basemap);
    mult = 1;
    for (int i = (int) len - 1; i >= 0; i--) {
        result += basemap[(std::size_t) in[i]] * mult;
        mult *= baselen;
    }
    return result;
}
unsigned long long atoi_dec(char* in, std::size_t len) {
    return atoi_base(in, len, "0123456789", 10);
}
unsigned long long atoi_bin(char* in, std::size_t len) {
    return atoi_base(in, len, "01", 2);
}
unsigned long long atoi_hex(char* in, std::size_t len) {
    return atoi_base(in, len, "0123456789ABCDEF", 16);
}
