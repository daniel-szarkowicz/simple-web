#include "http.h"

const char *methodstr(HttpMethod method) {
    switch (method) {
    case GET: return "GET";
    case HEAD: return "HEAD";
    case POST: return "POST";
    }
    __builtin_unreachable();
}
const char *httpstatusstr(HttpStatus status) {
    switch (status) {
    case NO_STATUS: return "NO_STATUS";
    case OK: return "OK";
    case SEE_OTHER: return "SEE_OTHER";
    case BAD_REQUEST: return "BAD_REQUEST";
    case UNAUTHORIZED: return "UNAUTHORIZED";
    case NOT_FOUND: return "NOT_FOUND";
    case METHOD_NOT_ALLOWED: return "METHOD_NOT_ALLOWED";
    case CONTENT_TOO_LARGE: return "CONTENT_TOO_LARGE";
    case INTERNAL_SERVER_ERROR: return "INTERNAL_SERVER_ERROR";
    case NOT_IMPLEMENTED: return "NOT_IMPLEMENTED";
    }
    __builtin_unreachable();
}
