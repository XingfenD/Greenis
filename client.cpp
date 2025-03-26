/**
 * @file ./client.cpp
 * @brief the client application that sends requests to the server and reads responses
 * @author Fendy (xingfen.star@gmail.com)
 * @version 1.0
 * @date 2025-03-13
 * @copyright Copyright (c) 2025
 */

/* stdlib */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

/* system */
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

/* C++ */
#include <vector>
#include <string>

/* proj */
#include <defs.h>
#include <query.h>
#include <err_pack.h>
#include <debug.h>

int main(int argc, char *argv[]) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        die("socket()");
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(PORT);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);  /* 127.0.0.1 */
    int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv) {
        die("connect");
    }

    /* multiple pipelined requests */
    std::vector<std::string> cmd;

    for (int i = 1; i < argc; i++) {
        cmd.push_back(argv[i]);
    }

    int32_t err = send_req(fd, cmd);

    if (err) {
        goto L_DONE;
    }

    err = read_res(fd);
    if (err) {
        goto L_DONE;
    }

L_DONE:
    close(fd);
    return 0;
}
