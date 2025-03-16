/**
 * @file ./server.cpp
 * @brief the server application that listens for incoming connections and processes client requests
 * @author Fendy (xingfen.star@gmail.com)
 * @version 1.0
 * @date 2025-03-13
 * @copyright Copyright (c) 2025
 */

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <err_pack.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <conn.h>
#include <debug.h>
#include <poll.h>

int main(int argc, char *argv[]) {
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
    addr.sin_port = htons(1230); /* Correct network byte order */
    addr.sin_addr.s_addr = INADDR_ANY; /* Correct address */
    int rv = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv) {
        die("bind()");
    }

    /* set the listen fd to nonblocking mode */
    fd_set_nb(fd);

    /* listen */
    rv = listen(fd, SOMAXCONN);
    if (rv) {
        die("listen()");
    }

    /* a map of all client connections, keyed by fd */
    std::vector<Conn *> fd2conn;
    /* the event loop */
    std::vector<struct pollfd> poll_args;

    while (true) {
        /* prepare the arguments of the poll() */
        poll_args.clear();

        /* put the listening sockets in the first position */
        struct pollfd pfd = {fd, POLLIN, 0};
        poll_args.push_back(pfd);

        /* the rest are the sockets that have already been connected */
        for (Conn *conn : fd2conn) {
            if (!conn) {
                continue;
            }

            /* always poll() for error */
            struct pollfd pfd = {conn->fd, POLLERR, 0};

            /* set poll() flags from the application's intent */
            if (conn->want_read) {
                pfd.events |= POLLIN;
            }
            if (conn->want_write) {
                pfd.events |= POLLOUT;
            }
            poll_args.push_back(pfd);
        }

        /* wait for readiness */
        int rv = poll(poll_args.data(), (nfds_t)poll_args.size(), -1);
        if (rv < 0 && errno == EINTR) {
            continue;   /* not an error */
        }
        if (rv < 0) {
            die("poll()");
        }

        /* handle the listening socket */
        if (poll_args[0].revents) {
            if (Conn *conn = handle_accept(fd)) {
                /* put it into the map */
                if (fd2conn.size() <= (size_t)conn->fd) {
                    fd2conn.resize(conn->fd + 1);
                }
                assert(!fd2conn[conn->fd]);
                fd2conn[conn->fd] = conn;
            }
        }

        /* handle connected sockets */
        /* NOTE: skip the first (listening socket) */
        for (size_t i = 1; i < poll_args.size(); ++i) {
            uint32_t ready = poll_args[i].revents;
            if (ready == 0) {
                continue;
            }

            Conn *conn = fd2conn[poll_args[i].fd];
            if (ready & POLLIN) {
                assert(conn->want_read);
                handle_read(conn);

                /* application logic */

            }
            if (ready & POLLOUT) {
                assert(conn->want_write);
                handle_write(conn);

                /* application logic */

            }

            /* close the socket from socket error or application logic */
            if ((ready & POLLERR) || conn->want_close) {
                (void)close(conn->fd);
                fd2conn[conn->fd] = NULL;
                delete conn;
            }
        } /* for each connected sockets */
    } /* the event loop */

    close(fd); /* Close the listening socket before exiting */
    return 0;
}
