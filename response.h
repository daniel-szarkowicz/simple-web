#pragma once

#include "posts.h"
#include "request.h"

void writestatus(int fd, HttpStatus status);

void onlystatus(int fd, HttpStatus status);

void respond(int fd, Request *req, Posts *posts);
