/**
 * @file ./inc/server/timer.h
 * @brief 
 * @author Fendy (xingfen.star@gmail.com)
 * @version 1.0
 * @date 2025-03-21
 * @copyright Copyright (c) 2025
 */
#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

uint64_t get_monotonic_msec();
uint32_t next_timer_ms();
void process_timers();

#endif /* !TIMER_H */