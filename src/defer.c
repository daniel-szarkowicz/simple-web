#include "defer.h"
#include <stdlib.h>

void _deferdata_free(_DeferData *defer) {
    free(defer->data);
    defer->size = 0;
    defer->cap = 0;
    defer->data = NULL;
}
jmp_buf *_deferdata_push(_DeferData *defer) {
    if (defer->size == defer->cap) {
        if (defer->cap == 0) defer->cap = 1;
        defer->cap *= 2;
        defer->data = realloc(defer->data, sizeof(jmp_buf) * defer->cap);
    }
    return &defer->data[defer->size++];
}
jmp_buf *_deferdata_pop(_DeferData *defer) {
    return &defer->data[--defer->size];
}
_DeferData _deferdata_create() {
    _DeferData defer = {0};
    // allocate space for the return jump at position 0
    _deferdata_push(&defer);
    return defer;
}
