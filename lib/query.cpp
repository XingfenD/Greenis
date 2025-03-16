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
#include <query.h>
#include <err_pack.h>
#include <vector>
#include <utils.h>
// #include <stdlib.h>
#include <debug.h>

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

int32_t send_req(int fd, const uint8_t *text, size_t len) {
    if (len > K_MAX_MSG) {
        return -1;
    }

    std::vector<uint8_t> wbuf;
    buf_append(wbuf, (const uint8_t *) &len, 4);
    buf_append(wbuf, text, len);
    return write_all(fd, wbuf.data(), wbuf.size());
}

int32_t read_res(int fd) {
    /* 4 bytes header */
    std::vector<uint8_t> rbuf;
    rbuf.resize(4);
    errno = 0;
    int32_t err = read_full(fd, rbuf.data(), 4);
    if (err) {
        if (errno == 0) {
            msgf("EOF, fd=%d\n", fd);
        } else {
            msgf("read() error, fd=%d\n", fd);
        }
        return err;
    }

    /* Parse length from header, assuming little endian */
    uint32_t len = 0;
    memcpy(&len, rbuf.data(), 4);
    if (len > K_MAX_MSG) {
        msgf("too long, len=%u\n", len);
        return -1;
    }

    /* Read message body */
    rbuf.resize(4 + len);
    err = read_full(fd, rbuf.data() + 4, len);
    if (err) {
        msg("read() error");
        return err;
    }

    /* Process the message */
    printf("len:%u data:%.*s\n", len, len < 100 ? len : 100, rbuf.data() + 4);
    return 0;
}
