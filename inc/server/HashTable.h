/**
 * @file ./inc/server/HashTable.h
 * @brief 
 * @author Fendy (xingfen.star@gmail.com)
 * @version 1.0
 * @date 2025-03-17
 * @copyright Copyright (c) 2025
 */

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stddef.h>
#include <stdint.h>


/* hashtable node, should be embedded into the payload */
struct HNode {
    HNode *next = NULL;
    uint64_t hcode = 0;
};

/* a simple fixed-sized hashtable */
struct HTab {
    HNode **tab = NULL; /* array of slots */
    size_t mask = 0;    /* power of 2 array size, 2^n - 1 */
    size_t size = 0;    /* number of keys */
};

/* the real hashtable interface */
/* it uses 2 hashtables for progressive rehashing */
struct HMap {
    HTab newer;
    HTab older;
    size_t migrate_pos = 0;
};

/* FNV hash */
inline uint64_t str_hash(const uint8_t *data, size_t len) {
    uint32_t h = 0x811C9DC5;
    for (size_t i = 0; i < len; i++) {
        h = (h + data[i]) * 0x01000193;
    }
    return h;
}

bool hnode_same(HNode *node, HNode *key);
HNode *hm_lookup(HMap *hmap, HNode *key, bool (*eq)(HNode *, HNode *));
void   hm_insert(HMap *hmap, HNode *node);
HNode *hm_delete(HMap *hmap, HNode *key, bool (*eq)(HNode *, HNode *));
void   hm_clear(HMap *hmap);
size_t hm_size(HMap *hmap);
/* invoke the callback on each node until it returns false */
void   hm_foreach(HMap *hmap, bool (*f)(HNode *, void *), void *arg);

#endif /* HASH_TABLE_H */
