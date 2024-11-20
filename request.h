#pragma once

#include <stdbool.h>

#include "http.h"

typedef struct {
    HttpMethod method;
    char fullpath[1024];
    char *path;
    char username[1024];
    bool loggedin;
    char posttext[2048];
    bool hasposttext;
} Request;

HttpStatus parse_method(char *method_str, HttpMethod *method);

HttpStatus parse_cookies(char *cookies, Request *request);

HttpStatus parse_request(int fd, Request *request);
