/**
 * @file ./lib/query.cpp
 * @brief Implements functions for reading and writing full data, and handling client requests
 * @author Fendy (xingfen.star@gmail.com)
 * @version 1.0
 * @date 2025-03-13
 * @copyright Copyright (c) 2025
 */

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

#include <vector>

#include <query.h>
#include <err_pack.h>
#include <utils.h>
// #include <stdlib.h>
#include <debug.h>

int32_t read_full(int fd, char *buf, size_t n) {
    while (n > 0) {
        ssize_t rv = read(fd, buf, n);
        if (rv <= 0) {
            return -1;  /* error, or unexpected EOF */
        }
        assert((size_t)rv <= n);
        n -= (size_t)rv;
        buf += rv;
    }
    return 0;
}

int32_t write_all(int fd, const char *buf, size_t n) {
    while (n > 0) {
        ssize_t rv = write(fd, buf, n);
        if (rv <= 0) {
            return -1;  /* error */
        }
        assert((size_t)rv <= n);
        n -= (size_t)rv;
        buf += rv;
    }
    return 0;
}

int32_t send_req(int fd, const std::vector<std::string> &cmd) {
    uint32_t len = 4;
    for (const std::string &s : cmd) {
        len += 4 + s.size();
    }
    if (len > K_MAX_MSG) {
        return -1;
    }

    char wbuf[4 + K_MAX_MSG];
    memcpy(&wbuf[0], &len, 4); /* assume little endian */
    uint32_t n = cmd.size();
    memcpy(&wbuf[4], &n, 4);
    size_t cur = 8;
    for (const std::string &s : cmd) {
        uint32_t p = (uint32_t)s.size();
        memcpy(&wbuf[cur], &p, 4);
        memcpy(&wbuf[cur + 4], s.data(), s.size());
        cur += 4 + s.size();
    }
    return write_all(fd, wbuf, 4 + len);
}

int32_t read_res(int fd) {
    /* 4 bytes header */
    char rbuf[4 + K_MAX_MSG + 1];
    errno = 0;
    int32_t err = read_full(fd, rbuf, 4);
    if (err) {
        if (errno == 0) {
            msgf("EOF, fd=%d\n", fd);
        } else {
            msgf("read() error, fd=%d\n", fd);
        }
        return err;
    }

    uint32_t len = 0;
    memcpy(&len, rbuf, 4); /* assume little endian */
    if (len > K_MAX_MSG) {
        msgf("too long, fd=%d\n", fd);
        return -1;
    }

    /* reply body */
    err = read_full(fd, &rbuf[4], len);
    if (err) {
        msgf("read() error, fd=%d\n", fd);
        return err;
    }

    /* print the result */
    uint32_t rescode = 0;
    if (len < 4) {
        msgf("bad response, fd=%d\n", fd);
        return -1;
    }
    memcpy(&rescode, &rbuf[4], 4);
    printf("server says: [%u] %.*s\n", rescode, len - 4, &rbuf[8]);
    return 0;
}
