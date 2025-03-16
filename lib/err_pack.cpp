/**
 * @file ./lib/err.cpp
 * @brief Implements error handling functions
 * @author Fendy (xingfen.star@gmail.com)
 * @version 1.0
 * @date 2025-03-13
 * @copyright Copyright (c) 2025
 */

#include <err_pack.h>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

void stream_printf(FILE *stream, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stream, format, args);
    va_end(args);
}

void msg(const char *msg) {
    stream_printf(stderr, "%s\n", msg);
}

void msgf(const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
}

void msg_errno(const char *msg) {
    stream_printf(stderr, "[errno:%d] %s\n", errno, msg);
}

void die(const char *msg) {
    int err = errno;
    stream_printf(stderr, "[%d] %s\n", err, msg);
    abort();
}
