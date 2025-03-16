/* C++ */
#include <vector>
#include <string>

/* proj */
#include <key_value.h>
#include <buffer.h>
#include <hashtable.h>
#include <defs.h>

struct {
    HMap db;    /* top-level hashtable */
} g_data;

/* equality comparison for `struct Entry` */
bool entry_eq(HNode *lhs, HNode *rhs) {
    struct Entry *le = container_of(lhs, struct Entry, node);
    struct Entry *re = container_of(rhs, struct Entry, node);
    return le->key == re->key;
}

/* FNV hash */
uint64_t str_hash(const uint8_t *data, size_t len) {
    uint32_t h = 0x811C9DC5;
    for (size_t i = 0; i < len; i++) {
        h = (h + data[i]) * 0x01000193;
    }
    return h;
}

void do_get(std::vector<std::string> &cmd, Buffer &out) {
    /* a dummy `Entry` just for the lookup */
    Entry key;
    key.key.swap(cmd[1]);
    key.node.hcode = str_hash((uint8_t *)key.key.data(), key.key.size());
    /* hashtable lookup */
    HNode *node = hm_lookup(&g_data.db, &key.node, &entry_eq);
    if (!node) {
        return out_nil(out);
    }
    /* copy the value */
    const std::string &val = container_of(node, Entry, node)->val;
    return out_str(out, val.data(), val.size());
}

void do_set(std::vector<std::string> &cmd, Buffer &out) {
    /* a dummy `Entry` just for the lookup */
    Entry key;
    key.key.swap(cmd[1]);
    key.node.hcode = str_hash((uint8_t *)key.key.data(), key.key.size());
    /* hashtable lookup */
    HNode *node = hm_lookup(&g_data.db, &key.node, &entry_eq);
    if (node) {
        /* found, update the value */
        container_of(node, Entry, node)->val.swap(cmd[2]);
    } else {
        /* not found, allocate & insert a new pair */
        Entry *ent = new Entry();
        ent->key.swap(key.key);
        ent->node.hcode = key.node.hcode;
        ent->val.swap(cmd[2]);
        hm_insert(&g_data.db, &ent->node);
    }
    return out_nil(out);
}

void do_del(std::vector<std::string> &cmd, Buffer &out) {
    /* a dummy `Entry` just for the lookup */
    Entry key;
    key.key.swap(cmd[1]);
    key.node.hcode = str_hash((uint8_t *)key.key.data(), key.key.size());
    /* hashtable delete */
    HNode *node = hm_delete(&g_data.db, &key.node, &entry_eq);
    if (node) { /* deallocate the pair */
        delete container_of(node, Entry, node);
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