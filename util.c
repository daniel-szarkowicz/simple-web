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
