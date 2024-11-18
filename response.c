#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "http.h"
#include "posts.h"
#include "response.h"

#define PRINTF(...) dprintf(fd, __VA_ARGS__)
void html_escape(int fd, char *str) {
    for (; *str != '\0'; ++str) {
        switch (*str) {
        case '<': dprintf(fd, "&lt;"); break;
        case '>': dprintf(fd, "&gt;"); break;
        case '&': dprintf(fd, "&amp;"); break;
        default: dprintf(fd, "%c", *str); break;
        }
    }
}
#define ESCAPE(str) html_escape(fd, str)

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

void post_redirect(int fd, char *loc) {
    writestatus(fd, SEE_OTHER);
    dprintf(fd, "Location: %s\r\n", loc);
}

void index_html(int fd, Request *req, Posts *posts) {
#include "build/index.htmlc"
}

void notfound(int fd) {
#include "build/notfound.htmlc"
}

void respond(int fd, Request *req, Posts *posts) {
    if (route(req, "/")) {
        if (req->method != GET) return onlystatus(fd, METHOD_NOT_ALLOWED);
        writestatus(fd, OK);
        contenthtml(fd);
        endheader(fd);
        index_html(fd, req, posts);
    } else if (route(req, "/login")) {
        if (req->method != POST) return onlystatus(fd, METHOD_NOT_ALLOWED);
        post_redirect(fd, "/");
        char encoded_username[sizeof(req->username)];
        url_encode(encoded_username, sizeof(encoded_username), req->username);
        printf("%s, %s\n", req->username, encoded_username);
        dprintf(fd, "Set-Cookie: username=%s\r\n", encoded_username);
        endheader(fd);
    } else if (route(req, "/logout")) {
        if (req->method != POST) return onlystatus(fd, METHOD_NOT_ALLOWED);
        post_redirect(fd, "/");
        dprintf(fd, "Set-Cookie: username=; Max-Age=0\r\n");
        endheader(fd);
    } else if (route(req, "/post")) {
        if (req->method != POST) return onlystatus(fd, METHOD_NOT_ALLOWED);
        if (!req->loggedin) return onlystatus(fd, UNAUTHORIZED);
        if (!req->hasposttext) return onlystatus(fd, BAD_REQUEST);

        Post *post = posts_reserve(posts);
        strncpy(post->username, req->username, sizeof(post->username));
        strncpy(post->posttext, req->posttext, sizeof(post->posttext));

        post_redirect(fd, "/");
        endheader(fd);
    } else {
        writestatus(fd, NOT_FOUND);
        contenthtml(fd);
        endheader(fd);
        notfound(fd);
    }
}
