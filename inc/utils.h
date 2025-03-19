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

    #ifdef __linux__

    /* stdlib */
    #include <stdint.h>

    /* C++ */
    #include <string>

    #endif /* __linux__ */

    #include <vector>

    void fd_set_nb(int fd);
    bool read_u32(const uint8_t *&cur, const uint8_t *end, uint32_t &out);
    bool read_str(const uint8_t *&cur, const uint8_t *end, size_t n, std::string &out);

#endif /* !UTILS_H */
