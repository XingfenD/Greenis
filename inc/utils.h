/**
 * @file ./inc/utils.h
 * @brief This file defines utility functions.
 * @author Fendy (xingfen.star@gmail.com)
 * @version 1.0
 * @date 2025-03-13
 * @copyright Copyright (c) 2025
 */

#ifndef UTILS_H
#define UTILS_H

    #include <vector>

    void buf_append(std::vector<uint8_t> &buf, const uint8_t *data, size_t len);
    void buf_consume(std::vector<uint8_t> &buf, size_t n);
    void fd_set_nb(int fd);

#endif /* UTILS_H */
