#include <stdio.h>
#include <string.h>

#define SEPARATOR '$'

int eof_or_sep(int c) { return c == EOF || c == SEPARATOR; }

void generate(FILE *in, FILE *out) {
    int c;
    while (1) {
        fprintf(out, "PRINTF(\"");
        while (!eof_or_sep(c = getc(in))) fprintf(out, "\\x%02x", c);
        fprintf(out, "\");\n");
        if (c == EOF) return;

        while (!eof_or_sep(c = getc(in))) fprintf(out, "%c", c);
        fprintf(out, "\n");
        if (c == EOF) return;
    }
}

int usage(char **argv) {
    printf("Usage: %s input output\n", argv[0]);
    return 1;
}

int main(int argc, char **argv) {
    if (argc == 1) {
        generate(stdin, stdout);
    } else if (argc == 3) {
        FILE *in = fopen(argv[1], "r");
        if (in == NULL) return usage(argv);
        FILE *out = fopen(argv[2], "w");
        if (out == NULL) return usage(argv);
        generate(in, out);
    } else {
        return usage(argv);
    }
}
