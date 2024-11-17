#pragma once

#include "http.h"
#include "util.h"

typedef struct {
    HttpMethod method;
    char fullpath[1024];
    char *path;
    char username[1024];
    char content[2048];
} Request;

HttpStatus parse_method(char *method_str, HttpMethod *method);

HttpStatus parse_cookies(char *cookies, Request *request);

HttpStatus parse_request(int fd, Request *request);
