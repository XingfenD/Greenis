/**
 * @file ./lib/server/key_value.cpp
 * @brief 
 * @author Fendy (xingfen.star@gmail.com)
 * @version 1.0
 * @date 2025-03-17
 * @copyright Copyright (c) 2025
 */

/* stdlib */
#include <assert.h>

/* C++ */
#include <vector>
#include <string>

/* proj */
/* proj::data structure */
#include <buffer.h>
#include <HashTable.h>
#include <heap.h>

#include <conn.h>
#include <timer.h>
#include <key_value.h>

#include <defs.h>
#include <utils.h>
// #include <list.h>
#include <global.h>


struct LookupKey {
    struct HNode node;  /* hashtable node */
    std::string key;
};

/* equality comparison for the top-level hashstable */
bool entry_eq(HNode *node, HNode *key) {
    struct Entry *ent = container_of(node, struct Entry, node);
    struct LookupKey *keydata = container_of(key, struct LookupKey, node);
    return ent->key == keydata->key;
}

Entry *entry_new(ValueType type) {
    Entry *ent = new Entry();
    ent->type = type;
    return ent;
}
void entry_del_sync(Entry *ent) {
    if (ent->type == T_ZSET) {
        zset_clear(&ent->zset);
    }
    delete ent;
}

void entry_del_func(void *arg) {
    entry_del_sync((Entry *)arg);
}

void entry_del(Entry *ent) {
    /* unlink it from any data structures */
    entry_set_ttl(ent, -1); /* remove from the heap data structure */
    /* run the destructor in a thread pool for large data structures */
    size_t set_size = (ent->type == T_ZSET) ? hm_size(&ent->zset.hmap) : 0;
    const size_t k_large_container_size = 1000;
    if (set_size > k_large_container_size) {
        thread_pool_queue(&g_data.thread_pool, &entry_del_func, ent);
    } else {
        entry_del_sync(ent);    /* small; avoid context switches */
    }
}

/* set or remove the TTL */
void entry_set_ttl(Entry *ent, int64_t ttl_ms) {
    if (ttl_ms < 0 && ent->heap_idx != (size_t)-1) {
        /* setting a negative TTL means removing the TTL */
        heap_delete(g_data.heap, ent->heap_idx);
        ent->heap_idx = -1;
    } else if (ttl_ms >= 0) {
        /* add or update the heap data structure */
        uint64_t expire_at = get_monotonic_msec() + (uint64_t)ttl_ms;
        heap_upsert(g_data.heap, ent->heap_idx, HeapItem{expire_at, &ent->heap_idx});
    }
}

void do_get(std::vector<std::string> &cmd, Buffer &out) {
    /* a dummy struct just for the lookup */
    LookupKey key;
    key.key.swap(cmd[1]);
    key.node.hcode = str_hash((uint8_t *)key.key.data(), key.key.size());

    /* hashtable lookup */
    HNode *node = hm_lookup(&g_data.db, &key.node, &entry_eq);
    if (!node) {
        return out_nil(out);
    }

    /* copy the value */
    Entry *ent = container_of(node, Entry, node);
    if (ent->type != T_STR) {
        return out_err(out, ERR_BAD_TYP, "not a string value");
    }
    return out_str(out, ent->str.data(), ent->str.size());
}

void do_set(std::vector<std::string> &cmd, Buffer &out) {
    /* a dummy struct just for the lookup */
    LookupKey key;
    key.key.swap(cmd[1]);
    key.node.hcode = str_hash((uint8_t *)key.key.data(), key.key.size());

    /* hashtable lookup */
    HNode *node = hm_lookup(&g_data.db, &key.node, &entry_eq);

    if (node) {
        /* found, update the value */
        Entry *ent = container_of(node, Entry, node);
        if (ent->type != T_STR) {
            return out_err(out, ERR_BAD_TYP, "a non-string value exists");
        }
        ent->str.swap(cmd[2]);
    } else {
        /* not found, allocate & insert a new pair */
        Entry *ent = entry_new(T_STR);
        ent->key.swap(key.key);
        ent->node.hcode = key.node.hcode;
        ent->str.swap(cmd[2]);
        hm_insert(&g_data.db, &ent->node);
    }
    return out_nil(out);
}

void do_del(std::vector<std::string> &cmd, Buffer &out) {
    /* a dummy struct just for the lookup */
    LookupKey key;
    key.key.swap(cmd[1]);
    key.node.hcode = str_hash((uint8_t *)key.key.data(), key.key.size());

    /* hashtable delete */
    HNode *node = hm_delete(&g_data.db, &key.node, &entry_eq);

    if (node) { /* deallocate the pair */
        entry_del(container_of(node, Entry, node));
    }
    return out_int(out, node ? 1 : 0);
}

bool cb_keys(HNode *node, void *arg) {
    Buffer &out = *(Buffer *)arg;
    const std::string &key = container_of(node, Entry, node)->key;
    out_str(out, key.data(), key.size());
    return true;
}

void do_keys(std::vector<std::string> &, Buffer &out) {
    out_arr(out, (uint32_t)hm_size(&g_data.db));
    hm_foreach(&g_data.db, &cb_keys, (void *)&out);
}

/* zadd zset score name */
void do_zadd(std::vector<std::string> &cmd, Buffer &out) {
    double score = 0;
    if (!str2dbl(cmd[2], score)) {
        return out_err(out, ERR_BAD_ARG, "expect float");
    }

    /* look up or create the zset */
    LookupKey key;
    key.key.swap(cmd[1]);
    key.node.hcode = str_hash((uint8_t *)key.key.data(), key.key.size());
    HNode *hnode = hm_lookup(&g_data.db, &key.node, &entry_eq);

    Entry *ent = NULL;
    if (!hnode) {   /* insert a new key */
        ent = entry_new(T_ZSET);
        ent->key.swap(key.key);
        ent->node.hcode = key.node.hcode;
        hm_insert(&g_data.db, &ent->node);
    } else {        /* check the existing key */
        ent = container_of(hnode, Entry, node);
        if (ent->type != T_ZSET) {
            return out_err(out, ERR_BAD_TYP, "expect zset");
        }
    }

    /* add or update the tuple */
    const std::string &name = cmd[3];
    bool added = zset_insert(&ent->zset, name.data(), name.size(), score);
    return out_int(out, (int64_t)added);
}

const ZSet k_empty_zset;

ZSet *expect_zset(std::string &s) {
    LookupKey key;
    key.key.swap(s);
    key.node.hcode = str_hash((uint8_t *)key.key.data(), key.key.size());
    HNode *hnode = hm_lookup(&g_data.db, &key.node, &entry_eq);
    if (!hnode) {   /* a non-existent key is treated as an empty zset */
        return (ZSet *)&k_empty_zset;
    }
    Entry *ent = container_of(hnode, Entry, node);
    return ent->type == T_ZSET ? &ent->zset : NULL;
}

/* zrem zset name */
void do_zrem(std::vector<std::string> &cmd, Buffer &out) {
    ZSet *zset = expect_zset(cmd[1]);
    if (!zset) {
        return out_err(out, ERR_BAD_TYP, "expect zset");
    }

    const std::string &name = cmd[2];
    ZNode *znode = zset_lookup(zset, name.data(), name.size());
    if (znode) {
        zset_delete(zset, znode);
    }
    return out_int(out, znode ? 1 : 0);
}

/* zscore zset name */
void do_zscore(std::vector<std::string> &cmd, Buffer &out) {
    ZSet *zset = expect_zset(cmd[1]);
    if (!zset) {
        return out_err(out, ERR_BAD_TYP, "expect zset");
    }

    const std::string &name = cmd[2];
    ZNode *znode = zset_lookup(zset, name.data(), name.size());
    return znode ? out_dbl(out, znode->score) : out_nil(out);
}

/* zquery zset score name offset limit */
void do_zquery(std::vector<std::string> &cmd, Buffer &out) {
    /* parse args */
    double score = 0;
    if (!str2dbl(cmd[2], score)) {
        return out_err(out, ERR_BAD_ARG, "expect fp number");
    }
    const std::string &name = cmd[3];
    int64_t offset = 0, limit = 0;
    if (!str2int(cmd[4], offset) || !str2int(cmd[5], limit)) {
        return out_err(out, ERR_BAD_ARG, "expect int");
    }

    /* get the zset */
    ZSet *zset = expect_zset(cmd[1]);
    if (!zset) {
        return out_err(out, ERR_BAD_TYP, "expect zset");
    }

    /* seek to the key */
    if (limit <= 0) {
        return out_arr(out, 0);
    }
    ZNode *znode = zset_seekge(zset, score, name.data(), name.size());
    znode = znode_offset(znode, offset);

    /* output */
    size_t ctx = out_begin_arr(out);
    int64_t n = 0;
    while (znode && n < limit) {
        out_str(out, znode->name, znode->len);
        out_dbl(out, znode->score);
        znode = znode_offset(znode, +1);
        n += 2;
    }
    out_end_arr(out, ctx, (uint32_t)n);
}

/* PEXPIRE key ttl_ms */
void do_expire(std::vector<std::string> &cmd, Buffer &out) {
    int64_t ttl_ms = 0;
    if (!str2int(cmd[2], ttl_ms)) {
        return out_err(out, ERR_BAD_ARG, "expect int64");
    }

    LookupKey key;
    key.key.swap(cmd[1]);
    key.node.hcode = str_hash((uint8_t *)key.key.data(), key.key.size());

    HNode *node = hm_lookup(&g_data.db, &key.node, &entry_eq);
    if (node) {
        Entry *ent = container_of(node, Entry, node);
        entry_set_ttl(ent, ttl_ms);
    }
    return out_int(out, node ? 1: 0);
}

/* PTTL key */
void do_ttl(std::vector<std::string> &cmd, Buffer &out) {
    LookupKey key;
    key.key.swap(cmd[1]);
    key.node.hcode = str_hash((uint8_t *)key.key.data(), key.key.size());

    HNode *node = hm_lookup(&g_data.db, &key.node, &entry_eq);
    if (!node) {
        return out_int(out, -2);    /* not found */
    }

    Entry *ent = container_of(node, Entry, node);
    if (ent->heap_idx == (size_t)-1) {
        return out_int(out, -1);    /* no TTL */
    }

    uint64_t expire_at = g_data.heap[ent->heap_idx].val;
    uint64_t now_ms = get_monotonic_msec();
    return out_int(out, expire_at > now_ms ? (expire_at - now_ms) : 0);
}
