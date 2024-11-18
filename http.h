#pragma once

typedef enum {
    GET,
    HEAD,
    // OPTIONS,
    // TRACE,
    // PUT,
    // DELETE,
    POST,
    // PATCH,
    // CONNECT,
} HttpMethod;

const char *methodstr(HttpMethod method);

typedef enum {
    NO_STATUS = 0,
    OK = 200,
    SEE_OTHER = 303,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    CONTENT_TOO_LARGE = 413,
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
} HttpStatus;

const char *httpstatusstr(HttpStatus status);

#define HTTP_TRY(expr)                                                         \
    do {                                                                       \
        HttpStatus __status__ = expr;                                          \
        if (__status__ != NO_STATUS) return __status__;                        \
    } while (0)
