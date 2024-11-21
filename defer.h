#pragma once

#include <setjmp.h>
#include <stddef.h>
// Heap allocated DEFER implementation using dynamic arrays
// supports unlimited number of DEFER statements

typedef struct {
    size_t size;
    size_t cap;
    jmp_buf *data;
} _DeferData;

void _deferdata_free(_DeferData *defer);

jmp_buf *_deferdata_push(_DeferData *defer);

jmp_buf *_deferdata_pop(_DeferData *defer);

_DeferData _deferdata_create();

// used to set up a function for DEFERring
#define DEFER_INIT()                                                           \
    _DeferData _defer = _deferdata_create();                                   \
    ((void)0)

// create a block of code that runs when the function RETURNs
// should not be used in loops
#define DEFER(block)                                                           \
    do {                                                                       \
        if (setjmp(*_deferdata_push(&_defer))) {                               \
            block;                                                             \
            longjmp(*_deferdata_pop(&_defer), 1);                              \
        }                                                                      \
    } while (0)

// return from a function with DEFERring
// needs to be used at the end of void functions
#define RETURN(block)                                                          \
    do {                                                                       \
        if (!setjmp(_defer.data[0])) longjmp(*_deferdata_pop(&_defer), 1);     \
        _deferdata_free(&_defer);                                              \
        return block;                                                          \
    } while (0)

/*
#include <limits.h>
// Stack allocated DEFER implementation,
// supports limited number of DEFER statements

#define DEFER_MAX_COUNT 10

typedef struct {
    unsigned char idx;
    jmp_buf stack[DEFER_MAX_COUNT];
} _Defer;

#define DEFER_INIT _DeferData _defer = {0}

#define DEFER(block) \
    do { \
        assert(_defer.idx != DEFER_MAX_COUNT); \
        if (setjmp(_defer.stack[++_defer.idx])) { \
            block; \
            longjmp(_defer.stack[_defer.idx--], 1); \
        } \
    } while (0)

#define RETURN \
    if (!setjmp(_defer.stack[0])) longjmp(_defer.stack[_defer.idx--], 1); \
    return
*/
