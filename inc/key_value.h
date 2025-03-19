#ifndef KEY_VALUE_H
#define KEY_VALUE_H

    #include <string>

    #include <HashTable.h>
    #include <buffer.h>

    /* KV pair for the top-level hashtable */
    struct Entry {
        struct HNode node;  /* hashtable node */
        std::string key;
        std::string val;
    };

    bool entry_eq(HNode *lhs, HNode *rhs);
    uint64_t str_hash(const uint8_t *data, size_t len);
    void do_get(std::vector<std::string> &cmd, Buffer &out);
    void do_set(std::vector<std::string> &cmd, Buffer &out);
    void do_del(std::vector<std::string> &cmd, Buffer &out);
    void do_keys(std::vector<std::string> &cmd, Buffer &out);

#endif /* !KEY_VALUE_H */
