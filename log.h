#pragma once

typedef enum {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
} LogLevel;

const char *loglevel_str(LogLevel level);

void log_set_level(LogLevel level);

LogLevel log_get_level();

void log_loc(const char *loc, LogLevel level, const char *fmt, ...)
    __attribute__((format(printf, 3, 4)));

#define STRINGIFY(x) STRINGIFY2(x)
#define STRINGIFY2(x) #x
#define l(level, ...)                                                          \
    log_loc(__FILE__ ":" STRINGIFY(__LINE__), level, __VA_ARGS__)
