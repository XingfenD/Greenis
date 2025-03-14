/**
 * @file ./inc/query.h
 * @brief Header file declaring functions for reading and writing full data, and handling client requests
 * @author Fendy (xingfen.star@gmail.com)
 * @version 1.0
 * @date 2025-03-13
 * @copyright Copyright (c) 2025
 */

#ifndef QUERY_H
#define QUERY_H

    #include <stdlib.h>

    #define K_MAX_MSG (32 << 20) /* likely larger than the kernel buffer */

    /**
     * @brief Reads exactly n bytes from the file descriptor fd into the buffer buf.
     *
     * This function attempts to read n bytes from the given file descriptor. It continues
     * reading until either all n bytes have been read, or an error occurs. If an error
     * occurs or an unexpected EOF is encountered, the function returns -1.
     *
     * @param fd The file descriptor to read from.
     * @param buf The buffer to read the data into.
     * @param n The number of bytes to read.
     * @return int32_t Returns 0 on success, or -1 on error or unexpected EOF.
     */
    int32_t read_full(int fd, uint8_t *buf, size_t n);

    /**
     * @brief Writes exactly n bytes from the buffer buf to the file descriptor fd.
     *
     * This function attempts to write n bytes to the given file descriptor. It continues
     * writing until either all n bytes have been written, or an error occurs. If an error
     * occurs, the function returns -1.
     *
     * @param fd The file descriptor to write to.
     * @param buf The buffer containing the data to write.
     * @param n The number of bytes to write.
     * @return int32_t Returns 0 on success, or -1 on error.
     */
    int32_t write_all(int fd, const uint8_t *buf, size_t n);

    /**
     * @brief Handle a query from a client.
     *
     * This function reads a query from the client, processes it, and sends a response back.
     * The communication protocol includes a 4-byte header indicating the length of the message body.
     *
     * @param connfd The file descriptor of the client connection.
     * @return int32_t 0 on success, -1 on error.
     */
    int32_t handle_query(int connfd);

    /**
     * @brief Send a query to the server and receive the response.
     *
     * This function sends a query to the server and waits for the response.
     * The communication protocol includes a 4-byte header indicating the length of the message body.
     *
     * @param fd The file descriptor of the server connection.
     * @param text The query text to send.
     * @return int32_t 0 on success, -1 on error.
     */
    int32_t query(int fd, const char *text);

    /* the `query` function was simply splited into `send_req` and `read_res` */
    int32_t send_req(int fd, const uint8_t *text, size_t len);

    int32_t read_res(int fd);

#endif /* QUERY_H */