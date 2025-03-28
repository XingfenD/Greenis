/**
 * @file ./inc/server/key_value.h
 * @brief 
 * @author Fendy (xingfen.star@gmail.com)
 * @version 1.0
 * @date 2025-03-17
 * @copyright Copyright (c) 2025
 */

#ifndef KEY_VALUE_H
#define KEY_VALUE_H

#include <string>

#include <HashTable.h>
#include <buffer.h>
#include <zset.h>
#include <defs.h>

/* KV pair for the top-level hashtable */
struct Entry {
    struct HNode node;  /* hashtable node */
    std::string key;    /* key */
    /* for TTL */
    size_t heap_idx = -1;   /* array index to the heap item */
    /* value */
    ValueType type = T_INIT;
    /* one of the following */
    std::string str;
    ZSet zset;
};

bool entry_eq(HNode *node, HNode *key);
void do_get(std::vector<std::string> &cmd, Buffer &out);
void do_set(std::vector<std::string> &cmd, Buffer &out);
void do_del(std::vector<std::string> &cmd, Buffer &out);
void do_keys(std::vector<std::string> &cmd, Buffer &out);
ZSet *expect_zset(std::string &s);
void do_zadd(std::vector<std::string> &cmd, Buffer &out);
void do_zrem(std::vector<std::string> &cmd, Buffer &out);
void do_zscore(std::vector<std::string> &cmd, Buffer &out);
void do_zquery(std::vector<std::string> &cmd, Buffer &out);
void do_expire(std::vector<std::string> &cmd, Buffer &out);
void do_ttl(std::vector<std::string> &cmd, Buffer &out);
void entry_del(Entry *ent);
void entry_set_ttl(Entry *ent, int64_t ttl_ms);

#endif /* !KEY_VALUE_H */
