#ifndef DEFS_H
#define DEFS_H

    #define K_MAX_ARGS ((size_t) (200 * 1000))

    #define K_MAX_MSG ((size_t) 4096)

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
