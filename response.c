#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "request.h"

bool route(Request *request, char *route) {
    return strcmp(request->path, route) == 0;
}

void contenthtml(int fd) { dprintf(fd, "Content-Type: text/html\r\n"); }
void endheader(int fd) { dprintf(fd, "\r\n"); }

void writestatus(int fd, HttpStatus status) {
    dprintf(fd, "HTTP/1.1 %d %s\r\n", status, httpstatusstr(status));
}

void onlystatus(int fd, HttpStatus status) {
    writestatus(fd, status);
    endheader(fd);
}


void notfound(int fd) {
#define HTML(str) dprintf(fd, "%s", str)
#include "build/notfound.htmlc"
#undef HTML
}

void respond(int fd, Request *req) {
    if (route(req, "/")) {
        if (req->method != GET) return onlystatus(fd, METHOD_NOT_ALLOWED);
        writestatus(fd, OK);
        contenthtml(fd);
        endheader(fd);
        dprintf(fd, "method: %d, %s<br>\n", req->method,
                methodstr(req->method));
        dprintf(fd, "path: %s<br>\n", req->fullpath);
        if (strlen(req->username) != 0) {
            dprintf(fd, "username: %s<br>\n", req->username);
        } else {
            dprintf(fd, "no username<br>\n");
        }
    } else if (route(req, "/login")) {
        if (req->method != POST) return onlystatus(fd, METHOD_NOT_ALLOWED);
        if (strlen(req->content) == 0) return onlystatus(fd, BAD_REQUEST);
        writestatus(fd, OK);
        dprintf(fd, "Set-Cookie: username=%s\r\n", req->content);
        endheader(fd);
    } else if (route(req, "/logout")) {
        if (req->method != POST) return onlystatus(fd, METHOD_NOT_ALLOWED);
        writestatus(fd, OK);
        dprintf(fd, "Set-Cookie: username=; Max-Age=0\r\n");
        dprintf(fd, "\r\n");
    } else if (route(req, "/post")) {
        if (req->method != POST) return onlystatus(fd, METHOD_NOT_ALLOWED);
        writestatus(fd, OK);
        endheader(fd);
    } else {
        writestatus(fd, NOT_FOUND);
        contenthtml(fd);
        endheader(fd);
        notfound(fd);
    }
}
