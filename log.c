#include "log.h"
#include <stdarg.h>
#include <stdio.h>

const char *loglevel_str(LogLevel level) {
    switch (level) {
    case TRACE: return "TRACE";
    case DEBUG: return "DEBUG";
    case INFO: return "INFO";
    case WARN: return "WARN";
    case ERROR: return "ERROR";
    }
    __builtin_unreachable();
}

static LogLevel log_level = WARN;

void log_set_level(LogLevel level) { log_level = level; }

LogLevel log_get_level() { return log_level; }

void log_loc(const char *loc, LogLevel level, const char *fmt, ...) {
    if (level < log_level) return;
    fprintf(stderr, "%s:", loc);
    fprintf(stderr, " [%s] ", loglevel_str(level));
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
}
