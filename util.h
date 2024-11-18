#pragma once

// chops off the first substring that matches delims,
// and returns part after the match or NULL if there was no match
char *strchop(char *str, char *delims);
