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

    #ifdef __linux__

    #include <string>
    #include <vector>

    #endif /* __linux */

    #include <stdlib.h>

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
    int32_t read_full(int fd, char *buf, size_t n);

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
    int32_t write_all(int fd, const char *buf, size_t n);

    int32_t send_req(int fd, const std::vector<std::string> &cmd);

    int32_t read_res(int fd);

    int32_t print_response(const uint8_t *data, size_t size);

#endif /* !QUERY_H */
