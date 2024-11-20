#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "http.h"
#include "log.h"
#include "posts.h"
#include "response.h"
#include "util.h"

#define HTML(str) dprintf(fd, "%s", str)
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

void starthtml(int fd) {
    contenthtml(fd);
    endheader(fd);
}

void route_root(int fd, Request *req, Posts *posts) {
    if (req->method != GET && req->method != HEAD) {
        l(ERROR, "Got method %s for path `%s`", methodstr(req->method),
          req->path);
        onlystatus(fd, METHOD_NOT_ALLOWED);
        return;
    } else {
        writestatus(fd, OK);
        starthtml(fd);
        if (req->method == GET) index_html(fd, req, posts);
    }
}

void route_login(int fd, Request *req) {
    if (req->method != POST) {
        l(ERROR, "Got method %s for path `%s`", methodstr(req->method),
          req->path);
        onlystatus(fd, METHOD_NOT_ALLOWED);
    } else if (!req->hasnewusername) {
        l(ERROR, "Got login but no username!");
        onlystatus(fd, BAD_REQUEST);
        return;
    } else {
        if (req->loggedin) {
            l(WARN, "User is already logged in! Old: `%s`, new: `%s`",
              req->username, req->newusername);
        }
        post_redirect(fd, "/");
        char encoded_username[sizeof(req->newusername)];
        url_encode(encoded_username, sizeof(encoded_username),
                   req->newusername);
        strncpy(req->username, req->newusername, sizeof(req->username));
        dprintf(fd, "Set-Cookie: username=%s\r\n", encoded_username);
        endheader(fd);
    }
}

void route_logout(int fd, Request *req) {
    if (req->method != POST) {
        l(ERROR, "Got method %s for path `%s`", methodstr(req->method),
          req->path);
        onlystatus(fd, METHOD_NOT_ALLOWED);
    } else {
        if (!req->loggedin) { l(WARN, "User is already logged out!"); }
        post_redirect(fd, "/");
        dprintf(fd, "Set-Cookie: username=; Max-Age=0\r\n");
        endheader(fd);
    }
}

void route_post(int fd, Request *req, Posts *posts) {
    if (req->method != POST) {
        l(ERROR, "Got method %s for path `%s`", methodstr(req->method),
          req->path);
        onlystatus(fd, METHOD_NOT_ALLOWED);
    } else if (!req->loggedin) {
        l(ERROR, "User is not logged in!");
        onlystatus(fd, UNAUTHORIZED);
    } else if (!req->hasposttext) {
        l(ERROR, "No post text!");
        onlystatus(fd, BAD_REQUEST);
    } else {
        Post *post = posts_reserve(posts);
        strncpy(post->username, req->username, sizeof(post->username));
        strncpy(post->posttext, req->posttext, sizeof(post->posttext));

        post_redirect(fd, "/");
        endheader(fd);
    }
}

void respond(int fd, Request *req, Posts *posts) {
    if (route(req, "/")) route_root(fd, req, posts);
    else if (route(req, "/login")) route_login(fd, req);
    else if (route(req, "/logout")) route_logout(fd, req);
    else if (route(req, "/post")) route_post(fd, req, posts);
    else {
        l(ERROR, "Invalid path `%s`", req->path);
        writestatus(fd, NOT_FOUND);
        contenthtml(fd);
        endheader(fd);
        notfound(fd);
    }
}
