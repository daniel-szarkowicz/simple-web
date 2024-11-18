#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *strchop(char *str, char *delims) {
    size_t idx = strcspn(str, delims);
    if (str[idx] == '\0') {
        return NULL;
    } else {
        str[idx] = '\0';
        return str + idx + 1;
    }
}

void url_decode(char *dest, size_t n, const char *src) {
    char *end = dest+n-1;
    for (; *src != '\0' && dest < end; ++src) {
        if (*src == '+') {
            *(dest++) = ' ';
        } else if (*src == '%' && strlen(src) > 2) {
            *(dest++) = strtol((char[]){src[1], src[2], 0}, NULL, 16);
            src += 2;
        } else {
            *(dest++) = *src;
        }
    }
    *dest = '\0';
}

void url_encode(char *dest, size_t n, const char *src) {
    for (; *src != '\0'; ++src) {
        int len = snprintf(dest, n, "%%%02x", *src & 0xff);
        if (len > n) return;
        dest += len;
        n -= len;
    }
}
