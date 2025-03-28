/**
 * @file ./inc/defs.h
 * @brief 
 * @author Fendy (xingfen.star@gmail.com)
 * @version 1.0
 * @date 2025-03-17
 * @copyright Copyright (c) 2025
 */

#ifndef DEFS_H
#define DEFS_H

    #define PORT 1230

    #define K_MAX_ARGS ((size_t) (200 * 1000))

    #define K_MAX_MSG ((size_t) 4096)

    #define K_IDLE_TIMEOUT_MS ((uint64_t) 5 * 1000)

    #define K_MAX_WORKS ((size_t) 2000)

    #define K_MAX_LOAD_FACTOR ((size_t) 8)

    #define K_REHASHING_WORK ((size_t) 128)

    #define container_of(ptr, T, member) \
        ((T *)( (char *)ptr - offsetof(T, member) ))

    /* error code for TAG_ERR */
    typedef enum {
        ERR_UNKNOWN = 1,    /* unknown command */
        ERR_TOO_BIG = 2,    /* response too big */
        ERR_BAD_TYP = 3,    /* unexpected value type */
        ERR_BAD_ARG = 4,    /* bad arguments */
    } ErrorCode;

    typedef enum {
        TAG_NIL = 0,    /* nil */
        TAG_ERR = 1,    /* error code + msg */
        TAG_STR = 2,    /* string */
        TAG_INT = 3,    /* int64 */
        TAG_DBL = 4,    /* double */
        TAG_ARR = 5,    /* array */
    } QueryTag;

    /* value types */
    typedef enum {
        T_INIT  = 0,
        T_STR   = 1,    /* string */
        T_ZSET  = 2,    /* sorted set */
    } ValueType;

#endif /* !DEFS_H */
