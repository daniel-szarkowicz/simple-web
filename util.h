#pragma once

#include <stddef.h>

// chops off the first substring that matches delims,
// and returns part after the match or NULL if there was no match
char *strchop(char *str, char *delims);

void url_decode(char *dest, size_t n, const char *src);
void url_encode(char *dest, size_t n, const char *src);
