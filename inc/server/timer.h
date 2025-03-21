#ifndef TIMER_H
#define TIMER_H

    #include <stdint.h>

    uint64_t get_monotonic_msec();
    int32_t next_timer_ms();
    void process_timers();

#endif /* !TIMER_H */