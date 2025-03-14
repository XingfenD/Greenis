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
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <query.h>
#include <err.h>
#include <vector>
#include <utils.h>
// #include <stdlib.h>

int32_t read_full(int fd, uint8_t *buf, size_t n) {
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

int32_t write_all(int fd, const uint8_t *buf, size_t n) {
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

int32_t handle_query(int connfd) {
    /* 4 bytes header */
    uint8_t rbuf[4 + K_MAX_MSG];
    errno = 0;
    int32_t err = read_full(connfd, rbuf, 4);
    if (err) {
        msg(errno == 0 ? "EOF" : "read() error");
        return err;
    }

    uint32_t len = 0;
    memcpy(&len, rbuf, 4); /* assume little endian */
    if (len > K_MAX_MSG) {
        msg("too long");
        return -1;
    }

    /* request body */
    err = read_full(connfd, &rbuf[4], len);
    if (err) {
        msg("read() error");
        return err;
    }

    /* do something */
    fprintf(stderr, "client says: %.*s\n", len, &rbuf[4]);

    /* reply using the same protocol */
    const char reply[] = "world";
    uint8_t wbuf[4 + sizeof(reply)];
    len = (uint32_t)strlen(reply);
    memcpy(wbuf, &len, 4);
    memcpy(&wbuf[4], reply, len);
    return write_all(connfd, wbuf, 4 + len);
}

int32_t query(int fd, const char *text) {
    uint32_t len = (uint32_t)strlen(text);
    if (len > K_MAX_MSG) {
        return -1;
    }

    uint8_t wbuf[4 + K_MAX_MSG];
    memcpy(wbuf, &len, 4);  /* assume little endian */
    memcpy(&wbuf[4], text, len);
    if (int32_t err = write_all(fd, wbuf, 4 + len)) {
        return err;
    }

    /* 4 bytes header */
    uint8_t rbuf[4 + K_MAX_MSG + 1];
    errno = 0;
    int32_t err = read_full(fd, rbuf, 4);
    if (err) {
        msg(errno == 0 ? "EOF" : "read() error");
        return err;
    }

    memcpy(&len, rbuf, 4);  /* assume little endian */
    if (len > K_MAX_MSG) {
        msg("too long");
        return -1;
    }

    /* reply body */
    err = read_full(fd, &rbuf[4], len);
    if (err) {
        msg("read() error");
        return err;
    }

    /* do something */
    printf("server says: %.*s\n", len, &rbuf[4]);
    return 0;
}

int32_t send_req(int fd, const uint8_t *text, size_t len) {
    if (len > K_MAX_MSG) {
        return -1;
    }

    std::vector<uint8_t> wbuf;
    buf_append(wbuf, (const uint8_t *)&len, 4);
    buf_append(wbuf, text, len);
    return write_all(fd, wbuf.data(), wbuf.size());
}

int32_t read_res(int fd) {
    // 4 bytes header
    std::vector<uint8_t> rbuf;
    rbuf.resize(4);
    errno = 0;
    int32_t err = read_full(fd, &rbuf[0], 4);
    if (err) {
        if (errno == 0) {
            msg("EOF");
        } else {
            msg("read() error");
        }
        return err;
    }

    uint32_t len = 0;
    memcpy(&len, rbuf.data(), 4);  // assume little endian
    if (len > K_MAX_MSG) {
        msg("too long");
        return -1;
    }

    // reply body
    rbuf.resize(4 + len);
    err = read_full(fd, &rbuf[4], len);
    if (err) {
        msg("read() error");
        return err;
    }

    // do something
    printf("len:%u data:%.*s\n", len, len < 100 ? len : 100, &rbuf[4]);
    return 0;
}