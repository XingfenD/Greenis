#ifndef DEFS_H
#define DEFS_H

    #define K_MAX_ARGS ((size_t) (200 * 1000))
    #define K_MAX_MSG ((size_t) 4096)
    #define container_of(ptr, T, member) \
    ((T *)( (char *)ptr - offsetof(T, member) ))

#endif /* !DEFS_H */
