/**
 * @file ./server.cpp
 * @brief 
 * @author Fendy (xingfen.star@gmail.com)
 * @version 1.0
 * @date 2025-03-13
 * @copyright Copyright (c) 2025
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

static void msg(const char *msg) {
    fprintf(stderr, "%s\n", msg);
}

static void die(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

static void do_something(int connfd) {
    char rbuf[64] = {};
    ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);
    if (n < 0) {
        msg("read() error");
        return;
    }
    if (n == 0) {
        msg("client closed connection");
        return;
    }
    rbuf[n] = '\0'; /* Ensure the string is null-terminated */
    fprintf(stderr, "client says: %s\n", rbuf);

    char wbuf[] = "world";
    write(connfd, wbuf, strlen(wbuf));
}

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        die("socket()");
    }

    /* this is needed for most server applications */
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    /* bind */
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234); /* Correct network byte order */
    addr.sin_addr.s_addr = INADDR_ANY; /* Correct address */
    int rv = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv) {
        die("bind()");
    }

    /* listen */
    rv = listen(fd, SOMAXCONN);
    if (rv) {
        die("listen()");
    }

    while (true) {
        /* accept */
        struct sockaddr_in client_addr = {};
        socklen_t socklen = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);
        if (connfd < 0) {
            msg("accept() error");
            continue;   /* error */
        }

        do_something(connfd);
        close(connfd);
    }

    close(fd); /* Close the listening socket before exiting */
    return 0;
}