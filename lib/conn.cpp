/**
 * @file ./lib/conn.cpp
 * @brief Implements the functionality of the Conn class
 * @author Fendy (xingfen.star@gmail.com)
 * @version 1.0
 * @date 2025-03-13
 * @copyright Copyright (c) 2025
 */

#include <conn.h>
#include <fcntl.h>
#include <cassert>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cerrno>
#include <utils.h>

/* Constructor initializes the connection object */
Conn::Conn() : fd(-1), want_read(false), want_write(false), want_close(false) {}

/* Destructor closes the file descriptor if it is valid */
Conn::~Conn() {
    if (fd >= 0) {
        close(fd);
    }
}

/* Move constructor transfers ownership of the file descriptor and other members */
Conn::Conn(Conn&& other) noexcept : fd(other.fd), want_read(other.want_read), want_write(other.want_read), want_close(other.want_close), incoming(std::move(other.incoming)), outgoing(std::move(other.outgoing)) {
    other.fd = -1;
}

/* Move assignment operator transfers ownership of the file descriptor and other members */
Conn& Conn::operator=(Conn&& other) noexcept {
    if (this != &other) {
        if (fd >= 0) {
            close(fd);
        }
        fd = other.fd;
        want_read = other.want_read;
        want_write = other.want_write;
        want_close = other.want_close;
        incoming = std::move(other.incoming);
        outgoing = std::move(other.outgoing);
        other.fd = -1;
    }
    return *this;
}

/* Handle an incoming connection from a listening socket */
Conn* Conn::handle_accept(int listen_fd) {
    struct sockaddr_in client_addr = {};
    socklen_t socklen = sizeof(client_addr);
    int connfd = accept(listen_fd, (struct sockaddr*)&client_addr, &socklen);
    if (connfd < 0) {
        perror("accept() error");
        return nullptr;
    }

    uint32_t ip = client_addr.sin_addr.s_addr;
    fprintf(stderr, "new client from %u.%u.%u.%u:%u\n",
        ip & 255, (ip >> 8) & 255, (ip >> 16) & 255, ip >> 24,
        ntohs(client_addr.sin_port));

    /* Set the new connection's file descriptor to non-blocking mode */
    int flags = fcntl(connfd, F_GETFL, 0);
    fcntl(connfd, F_SETFL, flags | O_NONBLOCK);

    /* Create a new Conn object for the new connection */
    Conn* conn = new Conn();
    conn->fd = connfd;
    conn->want_read = true;
    return conn;
}

/* Try to parse one request from the incoming buffer */
bool Conn::try_one_request() {
    /* Attempt to parse the protocol: message header */
    if (incoming.size() < 4) {
        return false; /* Need more data */
    }

    uint32_t len = 0;
    memcpy(&len, incoming.data(), 4);
    if (len > 1024) { /* Assume maximum message length is 1024 */
        fprintf(stderr, "too long\n");
        want_close = true;
        return false; /* Close the connection */
    }

    /* Message body */
    if (4 + len > incoming.size()) {
        return false; /* Need more data */
    }

    const uint8_t* request = &incoming[4];

    /* Process one request, execute application logic */
    printf("client says: len:%d data:%.*s\n",
        len, len < 100 ? len : 100, request);

    /* Generate response (echo) */
    buf_append(outgoing, (const uint8_t*)&len, 4);
    buf_append(outgoing, request, len);

    /* Remove processed request message */
    buf_consume(incoming, 4 + len);
    return true; /* Successfully processed one request */
}

/* Handle writing data to the socket */
void Conn::handle_write() {
    assert(outgoing.size() > 0);
    ssize_t rv = write(fd, outgoing.data(), outgoing.size());
    if (rv < 0 && errno == EAGAIN) {
        return; /* Not actually writable */
    }
    if (rv < 0) {
        perror("write() error");
        want_close = true; /* Error handling */
        return;
    }

    /* Remove written data from the outgoing buffer */
    buf_consume(outgoing, rv);

    /* Update readiness intentions */
    if (outgoing.size() == 0) { /* All data has been written */
        want_read = true;
        want_write = false;
    } /* Otherwise: still need to write */
}

/* Handle reading data from the socket */
void Conn::handle_read() {
    uint8_t buf[64 * 1024];
    ssize_t rv = read(fd, buf, sizeof(buf));
    if (rv < 0 && errno == EAGAIN) {
        return; /* Not actually readable */
    }
    if (rv < 0) {
        perror("read() error");
        want_close = true;
        return; /* Close the connection */
    }
    if (rv == 0) {
        if (incoming.size() == 0) {
            fprintf(stderr, "client closed\n");
        } else {
            fprintf(stderr, "unexpected EOF\n");
        }
        want_close = true;
        return; /* Close the connection */
    }

    /* Append new data to the incoming buffer */
    incoming.insert(incoming.end(), buf, buf + rv);

    /* Parse requests and generate responses */
    while (try_one_request()) {}

    /* Update readiness intentions */
    if (outgoing.size() > 0) { /* Have response data */
        want_read = false;
        want_write = true;
        /* The socket may already be ready for writing, in request-response protocols,
         * try to write immediately rather than waiting for the next iteration. */
        handle_write();
    } /* Otherwise: still need to read */
}
