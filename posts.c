#include <stdlib.h>

#include "posts.h"

Post *posts_reserve(Posts *self) {
    if (self->count == self->capacity) {
        if (self->capacity == 0) self->capacity = 1;
        self->capacity *= 2;
        self->data = (typeof(self->data))realloc(
            self->data, sizeof(*self->data) * self->capacity);
    }
    return &self->data[self->count++];
}

void posts_push(Posts *self, Post post) { *posts_reserve(self) = post; }

void posts_free(Posts *self) {
    free(self->data);
    self->count = 0;
    self->capacity = 0;
}
