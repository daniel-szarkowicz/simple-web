#pragma once

#include <stddef.h>

typedef struct {
    char username[1024];
    char posttext[2048];
} Post;

typedef struct {
    Post *data;
    size_t count;
    size_t capacity;
} Posts;

Post *posts_reserve(Posts *self);

void posts_push(Posts *self, Post post);

void posts_free(Posts *self);
