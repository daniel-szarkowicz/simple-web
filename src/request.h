#pragma once

#include <stdbool.h>

#include "http.h"

#define USERNAME_MIN_LEN 2
#define USERNAME_MAX_LEN 50
#define POST_MIN_LEN 1
#define POST_MAX_LEN 500

typedef struct {
    HttpMethod method;
    char fullpath[1024];
    char *path;
    char username[USERNAME_MAX_LEN + 1];
    bool loggedin;
    char newusername[USERNAME_MAX_LEN + 1];
    bool hasnewusername;
    char posttext[POST_MAX_LEN + 1];
    bool hasposttext;
} Request;

HttpStatus parse_method(char *method_str, HttpMethod *method);

HttpStatus parse_cookies(char *cookies, Request *request);

HttpStatus parse_request(int fd, Request *request);
