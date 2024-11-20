#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "http.h"
#include "log.h"
#include "request.h"
#include "util.h"

HttpStatus parse_method(char *method_str, HttpMethod *method) {
    if (strcmp(method_str, "GET") == 0) *method = GET;
    else if (strcmp(method_str, "HEAD") == 0) *method = HEAD;
    else if (strcmp(method_str, "POST") == 0) *method = POST;
    else {
        l(ERROR, "Got unknown HTTP method: %s", method_str);
        return NOT_IMPLEMENTED;
    }
    return NO_STATUS;
}

HttpStatus parse_cookies(char *cookies, Request *request) {
    char *rest = strchop(cookies, ";");
    char *value = strchop(cookies, "=");
    if (value == NULL) {
        l(ERROR, "Got badly formatted Cookie, key: `%s`", cookies);
        return BAD_REQUEST;
    };
    if (strcmp(cookies, "username") == 0) {
        url_decode(request->username, sizeof(request->username), value);
        request->loggedin = true;
    }
    if (rest == NULL) return NO_STATUS;
    else return parse_cookies(rest, request);
}

HttpStatus parse_content(char *content, Request *request) {
    if (strlen(content) == 0) return NO_STATUS;
    char *rest = strchop(content, "&");
    char *value = strchop(content, "=");
    if (value == NULL) {
        l(ERROR, "Got badly formatted content, key: `%s`", content);
        return BAD_REQUEST;
    };
    if (strcmp(content, "username") == 0) {
        url_decode(request->newusername, sizeof(request->newusername), value);
        request->hasnewusername = true;
    }
    if (strcmp(content, "posttext") == 0) {
        url_decode(request->posttext, sizeof(request->posttext), value);
        request->hasposttext = true;
    }
    if (rest == NULL) return NO_STATUS;
    else return parse_content(rest, request);
}

static const size_t LINE_LEN = 2048;

HttpStatus parse_request(int fd, Request *request) {
    FILE *stream = fdopen(fd, "r");
    char linebuf[LINE_LEN + 1];

    if (fgets(linebuf, LINE_LEN, stream) == NULL) {
        l(ERROR, "Failed to read request line, got NULL");
        return BAD_REQUEST;
    }
    if (strstr("linebuf", "\r\n") == NULL) {
        l(ERROR, "Request line does not contain \\r\\n");
        return BAD_REQUEST;
    }

    char method[16];
    static_assert(sizeof(request->fullpath) == 1024,
                  "request path size is wrong");
    char version[16];
    if (sscanf(linebuf, "%15s %1023s %15s", method, request->fullpath,
               version) != 3) {
        l(ERROR, "Got malformed request line: `%s`", linebuf);
        return BAD_REQUEST;
    }
    request->path = request->fullpath;
    HTTP_TRY(parse_method(method, &request->method));
    // TODO: validate version

    size_t content_length = 0;
    while (true) {
        if (fgets(linebuf, LINE_LEN, stream) == NULL) return BAD_REQUEST;
        if (strstr("linebuf", "\r\n") == NULL) {
            l(ERROR, "Request line does not contain \\r\\n");
            return BAD_REQUEST;
        }
        if (strcmp(linebuf, "\r\n") == 0) break;

        char key[128];
        char value[1024];
        if (sscanf(linebuf, "%127[^:]: %1023[^(\r\n)]", key, value) != 2) {
            l(ERROR, "Got malformed header line: `%s`", linebuf);
            return BAD_REQUEST;
        }
        if (strcmp(key, "Content-Length") == 0) {
            if (sscanf(value, "%zu", &content_length) != 1) {
                l(ERROR, "Could not parse Content-Length: `%s`", value);
                return BAD_REQUEST;
            }
        } else if (strcmp(key, "Cookie") == 0) {
            HTTP_TRY(parse_cookies(value, request));
        }
    }
    char content[2048];
    if (content_length > sizeof(content) - 1) {
        l(ERROR, "Content-Length is too large: %zu (max: %zu)", content_length,
          sizeof(content));
        return CONTENT_TOO_LARGE;
    }

    size_t read_bytes = fread(content, 1, content_length, stream);
    if (read_bytes != content_length) {
        l(ERROR, "Expected %zu bytes, but only got %zu", content_length,
          read_bytes);
        return BAD_REQUEST;
    }
    HTTP_TRY(parse_content(content, request));
    return NO_STATUS;
}
