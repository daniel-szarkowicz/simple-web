#pragma once

#include "posts.h"
#include "request.h"

void writestatus(int fd, HttpStatus status);

void statuspage(int fd, HttpStatus status);

void respond(int fd, Request *req, Posts *posts);
