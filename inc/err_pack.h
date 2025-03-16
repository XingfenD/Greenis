/**
 * @file ./inc/err_pack.h
 * @brief Header file declaring error handling functions
 * @author Fendy (xingfen.star@gmail.com)
 * @version 1.0
 * @date 2025-03-13
 * @copyright Copyright (c) 2025
 */

#ifndef ERR_PACK_H
#define ERR_PACK_H

    #include <errno.h>
    #include <cerrno>
    #include <cassert>
    #include <cstdio>

    void stream_printf(FILE *stream, const char *format, ...);
    void msg(const char *msg);
    void msgf(const char *msg, ...);
    void msg_errno(const char *msg);
    void die(const char *msg);

#endif /* !ERR_PACK_H */
