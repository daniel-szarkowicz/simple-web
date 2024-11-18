#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "request.h"

HttpStatus parse_method(char *method_str, HttpMethod *method) {
    if (strcmp(method_str, "GET") == 0) *method = GET;
    else if (strcmp(method_str, "HEAD") == 0) *method = HEAD;
    else if (strcmp(method_str, "POST") == 0) *method = POST;
    else return NOT_IMPLEMENTED;
    return NO_STATUS;
}

HttpStatus parse_cookies(char *cookies, Request *request) {
    char *rest = strchop(cookies, ";");
    char *value = strchop(cookies, "=");
    if (value == NULL) return BAD_REQUEST;
    if (strcmp(cookies, "username") == 0) {
        strncpy(request->username, value, sizeof(request->username));
        request->loggedin = true;
    }
    if (rest == NULL) return NO_STATUS;
    else return parse_cookies(rest, request);
}

HttpStatus parse_content(char *content, Request *request) {
    if (strlen(content) == 0) return NO_STATUS;
    char *rest = strchop(content, "&");
    char *value = strchop(content, "=");
    if (value == NULL) return BAD_REQUEST;
    if (strcmp(content, "username") == 0) {
        strncpy(request->username, value, sizeof(request->username));
        request->loggedin = true;
    }
    if (rest == NULL) return NO_STATUS;
    else return parse_content(rest, request);
}

static const size_t LINE_LEN = 2048;

HttpStatus parse_request(int fd, Request *request) {
    FILE *stream = fdopen(fd, "r");
    char linebuf[LINE_LEN + 1];

    if (fgets(linebuf, LINE_LEN, stream) == NULL) return BAD_REQUEST;
    char method[16];
    static_assert(sizeof(request->fullpath) == 1024,
                  "request path size is wrong");
    char version[16];
    if (sscanf(linebuf, "%15s %1023s %15s", method, request->fullpath,
               version) != 3) {
        return BAD_REQUEST;
    }
    request->path = request->fullpath;
    HTTP_TRY(parse_method(method, &request->method));
    // TODO: validate version

    size_t content_length = 0;
    while (true) {
        if (fgets(linebuf, LINE_LEN, stream) == NULL) return BAD_REQUEST;
        if (strcmp(linebuf, "\r\n") == 0) break;

        char key[128];
        char value[1024];
        if (sscanf(linebuf, "%127[^:]: %1023[^(\r\n)]", key, value) != 2) {
            return BAD_REQUEST;
        }
        if (strcmp(key, "Content-Length") == 0) {
            if (sscanf(value, "%zu", &content_length) != 1) {
                return BAD_REQUEST;
            }
        } else if (strcmp(key, "Cookie") == 0) {
            HTTP_TRY(parse_cookies(value, request));
        }
    }
    if (content_length > sizeof(request->content) - 1) {
        return CONTENT_TOO_LARGE;
    }
    if (fread(request->content, 1, content_length, stream) != content_length) {
        return BAD_REQUEST;
    }
    HTTP_TRY(parse_content(request->content, request));
    return NO_STATUS;
}
