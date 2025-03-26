/**
 * @file ./lib/server/conn.cpp
 * @brief Implements the functionality of the Conn class
 * @author Fendy (xingfen.star@gmail.com)
 * @version 1.0
 * @date 2025-03-13
 * @copyright Copyright (c) 2025
 */

#ifdef __linux__

#include <string.h>

 #endif /* __linux */

/* stdlib */
#include <unistd.h>

/* system */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

/* proj */
#include <conn.h>
#include <err_pack.h>
#include <utils.h>
#include <defs.h>
#include <response.h>
#include <HashTable.h>
#include <timer.h>
#include <global.h>

/* application callback when the listening socket is ready */
int32_t handle_accept(int fd) {
    /* accept */
    struct sockaddr_in client_addr = {};
    socklen_t socklen = sizeof(client_addr);
    int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);
    if (connfd < 0) {
        msg_errno("accept() error");
        return -1;
    }

    uint32_t ip = client_addr.sin_addr.s_addr;
    stream_printf(stderr, "new client from %u.%u.%u.%u:%u\n",
        ip & 255, (ip >> 8) & 255, (ip >> 16) & 255, ip >> 24,
        ntohs(client_addr.sin_port)
    );

    /* set the new connection fd to nonblocking mode */
    fd_set_nb(connfd);

    /* create a `struct Conn` */
    Conn *conn = new Conn();
    conn->fd = connfd;
    conn->want_read = true;
    conn->last_active_ms = get_monotonic_msec();
    dlist_insert_before(&g_data.idle_list, &conn->idle_node);

    /* put it into the map */
    if (g_data.fd2conn.size() <= (size_t)conn->fd) {
        g_data.fd2conn.resize(conn->fd + 1);
    }
    assert(!g_data.fd2conn[conn->fd]);
    g_data.fd2conn[conn->fd] = conn;
    printf("g_data.fd2conn.size(): %zu\n", g_data.fd2conn.size());
    return 0;
}

/* process 1 request if there is enough data */
bool try_one_request(Conn *conn) {
    /* try to parse the protocol: message header */
    if (conn->incoming.size() < 4) {
        return false;   /* want read */
    }

    uint32_t len = 0;
    memcpy(&len, conn->incoming.data(), 4);
    if (len > K_MAX_MSG) {
        msgf("too long, len=%u\n", len);
        conn->want_close = true;
        return false;   /* want close */
    }

    /* message body */
    if (4 + len > conn->incoming.size()) {
        return false;   /* want read */
    }

    const uint8_t *request = &conn->incoming[4];

    /* got one request, do some application logic */
    std::vector<std::string> cmd;
    if (parse_req(request, len, cmd) < 0) {
        msg("bad request");
        conn->want_close = true;
        return false;   /* want close */
    }
    size_t header_pos = 0;
    response_begin(conn->outgoing, &header_pos);
    do_request(cmd, conn->outgoing);
    response_end(conn->outgoing, header_pos);

    /* application logic done! remove the request message. */
    buf_consume(conn->incoming, 4 + len);
    /* NOTE: Q Why not just empty the buffer? See the explanation of "pipelining". */
    return true; /* success */
}

/* application callback when the socket is writable */
void handle_write(Conn *conn) {
    assert(conn->outgoing.size() > 0);
    ssize_t rv = write(conn->fd, &conn->outgoing[0], conn->outgoing.size());
    if (rv < 0 && errno == EAGAIN) {
        return; /* actually not ready */
    }
    if (rv < 0) {
        msg_errno("write() error");
        conn->want_close = true;    /* error handling */
        return;
    }

    /* remove written data from `outgoing` */
    buf_consume(conn->outgoing, (size_t)rv);

    /* update the readiness intention */
    if (conn->outgoing.size() == 0) {   /* all data written */
        conn->want_read = true;
        conn->want_write = false;
    } /* else: want write */
}

/* application callback when the socket is readable */
void handle_read(Conn *conn) {
    /* read some data */
    uint8_t buf[64 * 1024];
    ssize_t rv = read(conn->fd, buf, sizeof(buf));
    if (rv < 0 && errno == EAGAIN) {
        return; /* actually not ready */
    }

    /* handle IO error */
    if (rv < 0) {
        msg_errno("read() error");
        conn->want_close = true;
        return; /* want close */
    }

    /* handle EOF */
    if (rv == 0) {
        if (conn->incoming.size() == 0) {
            msg("client closed");
        } else {
            msg("unexpected EOF");
        }
        conn->want_close = true;
        return; /* want close */
    }

    /* got some new data */
    buf_append(conn->incoming, buf, (size_t)rv);

    /* parse requests and generate responses */
    while (try_one_request(conn)) {}
    /* NOTE: Q Why calling this in a loop? See the explanation of "pipelining". */

    /* update the readiness intention */
    if (conn->outgoing.size() > 0) {    /* has a response */
        conn->want_read = false;
        conn->want_write = true;
        /* The socket is likely ready to write in a request-response protocol,
           try to write it without waiting for the next iteration. */
        return handle_write(conn);
    }   /* else: want read */
}

void conn_destroy(Conn *conn) {
    (void)close(conn->fd);
    g_data.fd2conn[conn->fd] = NULL;
    dlist_detach(&conn->idle_node);
    delete conn;
}
