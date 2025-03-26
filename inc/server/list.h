/**
 * @file ./inc/server/list.h
 * @brief 
 * @author Fendy (xingfen.star@gmail.com)
 * @version 1.0
 * @date 2025-03-21
 * @copyright Copyright (c) 2025
 */
#ifndef LIST_H
#define LIST_H

#include <stddef.h>

struct DList {
    DList *prev = NULL;
    DList *next = NULL;
};

inline void dlist_init(DList *node) {
    node->prev = node->next = node;
}

inline bool dlist_empty(DList *node) {
    return node->next == node;
}

inline void dlist_detach(DList *node) {
    DList *prev = node->prev;
    DList *next = node->next;
    prev->next = next;
    next->prev = prev;
}

inline void dlist_insert_before(DList *target, DList *rookie) {
    DList *prev = target->prev;
    prev->next = rookie;
    rookie->prev = prev;
    rookie->next = target;
    target->prev = rookie;
}

#endif /* LIST_H */
