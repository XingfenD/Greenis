/**
 * @file ./lib/server/timer.cpp
 * @brief 
 * @author Fendy (xingfen.star@gmail.com)
 * @version 1.0
 * @date 2025-03-21
 * @copyright Copyright (c) 2025
 */

#include <timer.h>
#include <sys/time.h>
#include <conn.h>
#include <HashTable.h>
#include <defs.h>
#include <global.h>

uint64_t get_monotonic_msec() {
    struct timespec tv = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &tv);
    return uint64_t(tv.tv_sec) * 1000 + tv.tv_nsec / 1000 / 1000;
}

const uint64_t k_idle_timeout_ms = 5 * 1000;

int32_t next_timer_ms() {
    if (dlist_empty(&g_data.idle_list)) {
        return -1;  /* no timers, no timeouts */
    }

    uint64_t now_ms = get_monotonic_msec();
    Conn *conn = container_of(g_data.idle_list.next, Conn, idle_node);
    uint64_t next_ms = conn->last_active_ms + k_idle_timeout_ms;
    if (next_ms <= now_ms) {
        return 0;   /* missed? */
    }
    return (int32_t)(next_ms - now_ms);
}

void process_timers() {
    uint64_t now_ms = get_monotonic_msec();
    while (!dlist_empty(&g_data.idle_list)) {
        Conn *conn = container_of(g_data.idle_list.next, Conn, idle_node);
        uint64_t next_ms = conn->last_active_ms + k_idle_timeout_ms;
        if (next_ms >= now_ms) {
            break;  /* not expired */
        }

        fprintf(stderr, "removing idle connection: %d\n", conn->fd);
        conn_destroy(conn);
    }
}