/**
 * @file ./inc/server/zset.h
 * @brief 
 * @author Fendy (xingfen.star@gmail.com)
 * @version 1.0
 * @date 2025-03-19
 * @copyright Copyright (c) 2025
 */

#ifndef ZSET_H
#define ZSET_H

#include <avl.h>
#include <HashTable.h>

struct ZSet {
    AVLNode *root = NULL;   /* index by (score, name) */
    HMap hmap;              /* index by name */
};

struct ZNode {
    AVLNode tree;
    HNode   hmap;
    double  score = 0;
    size_t  len = 0;
    char    name[0];        /* flexible array */
};

bool   zset_insert(ZSet *zset, const char *name, size_t len, double score);
ZNode *zset_lookup(ZSet *zset, const char *name, size_t len);
void   zset_delete(ZSet *zset, ZNode *node);
ZNode *zset_seekge(ZSet *zset, double score, const char *name, size_t len);
void   zset_clear(ZSet *zset);
ZNode *znode_offset(ZNode *node, int64_t offset);

#endif /* ZSET_H */
