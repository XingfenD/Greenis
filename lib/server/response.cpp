/**
 * @file ./lib/response.cpp
 * @brief Implements request parsing, command handling, and response generation for a key-value store.
 * @author Fendy (xingfen.star@gmail.com)
 * @version 1.0
 * @date 2025-03-16
 * @copyright Copyright (c) 2025
 */

#ifdef __linux__

#include <string.h>

#endif /* __linux */

/* stdlib */
#include <stdint.h>

/* proj */
#include <map>
#include <string>
#include <vector>

/* proj */
#include <utils.h>
#include <response.h>
#include <buffer.h>
#include <defs.h>
#include <key_value.h>

/*
 * +------+-----+------+-----+------+-----+-----+------+
 * | nstr | len | str1 | len | str2 | ... | len | strn |
 * +------+-----+------+-----+------+-----+-----+------+
 */
int32_t
parse_req(const uint8_t *data, size_t size, std::vector<std::string> &out) {
    const uint8_t *end = data + size;
    uint32_t nstr = 0;
    if (!read_u32(data, end, nstr)) {
        return -1;
    }
    if (nstr > K_MAX_ARGS) {
        return -1;  /* safety limit */
    }

    while (out.size() < nstr) {
        uint32_t len = 0;
        if (!read_u32(data, end, len)) {
            return -1;
        }
        out.push_back(std::string());
        if (!read_str(data, end, len, out.back())) {
            return -1;
        }
    }

    if (data != end) {
        return -1;  /* trailing garbage */
    }

    return 0;
}

void do_request(std::vector<std::string> &cmd, Buffer &out) {
    if (cmd.size() == 2 && cmd[0] == "get") {
        return do_get(cmd, out);
    } else if (cmd.size() == 3 && cmd[0] == "set") {
        return do_set(cmd, out);
    } else if (cmd.size() == 2 && cmd[0] == "del") {
        return do_del(cmd, out);
    } else if (cmd.size() == 1 && cmd[0] == "keys") {
        return do_keys(cmd, out);
    } else if (cmd.size() == 4 && cmd[0] == "zadd") {
        return do_zadd(cmd, out);
    } else if (cmd.size() == 3 && cmd[0] == "zrem") {
        return do_zrem(cmd, out);
    } else if (cmd.size() == 3 && cmd[0] == "zscore") {
        return do_zscore(cmd, out);
    } else if (cmd.size() == 6 && cmd[0] == "zquery") {
        return do_zquery(cmd, out);
    } else {
        return out_err(out, ERR_UNKNOWN, "unknown command.");
    }
}

void response_begin(Buffer &out, size_t *header) {
    *header = out.size();       /* messege header position */
    buf_append_u32(out, 0);     /* reserve space */
}

size_t response_size(Buffer &out, size_t header) {
    return out.size() - header - 4;
}

void response_end(Buffer &out, size_t header) {
    size_t msg_size = response_size(out, header);
    if (msg_size > K_MAX_MSG) {
        out.resize(header + 4);
        out_err(out, ERR_TOO_BIG, "response is too big.");
        msg_size = response_size(out, header);
    }
    /* message header */
    uint32_t len = (uint32_t) msg_size;
    memcpy(&out[header], &len, 4);
}
