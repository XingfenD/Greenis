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
        std::string key;
        // std::string val;

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

#endif /* !KEY_VALUE_H */
