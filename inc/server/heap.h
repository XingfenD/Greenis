/**
 * @file ./inc/server/heap.h
 * @brief 
 * @author Fendy (xingfen.star@gmail.com)
 * @version 1.0
 * @date 2025-03-27
 * @copyright Copyright (c) 2025
 */

#ifndef HEAP_H
#define HEAP_H

#include <stddef.h>
#include <stdint.h>
#include <vector>


struct HeapItem {
    uint64_t val = 0;
    size_t *ref = NULL;
};

void heap_update(HeapItem *a, size_t pos, size_t len);
void heap_delete(std::vector<HeapItem> &a, size_t pos);
void heap_upsert(std::vector<HeapItem> &a, size_t pos, HeapItem t);

#endif /* HEAP_H */