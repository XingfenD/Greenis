/**
 * @file ./lib/response.cpp
 * @brief Implements request parsing, command handling, and response generation for a key-value store.
 * @author Fendy (xingfen.star@gmail.com)
 * @version 1.0
 * @date 2025-03-16
 * @copyright Copyright (c) 2025
 */

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#include <utils.h>
#include <response.h>

/* placeholder; implemented later */
std::map<std::string, std::string> g_data;

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

void do_request(std::vector<std::string> &cmd, Response &out) {
    if (cmd.size() == 2 && cmd[0] == "get") {
        auto it = g_data.find(cmd[1]);
        if (it == g_data.end()) {
            out.status = RES_NX;    /* not found */
            return;
        }
        const std::string &val = it->second;
        out.data.assign(val.begin(), val.end());
    } else if (cmd.size() == 3 && cmd[0] == "set") {
        g_data[cmd[1]].swap(cmd[2]);
    } else if (cmd.size() == 2 && cmd[0] == "del") {
        g_data.erase(cmd[1]);
    } else {
        out.status = RES_ERR;       /* unrecognized command */
    }
}

void make_response(const Response &resp, std::vector<uint8_t> &out) {
    uint32_t resp_len = 4 + (uint32_t) resp.data.size();
    buf_append(out, (const uint8_t *) &resp_len, 4);
    buf_append(out, (const uint8_t *) &resp.status, 4);
    buf_append(out, resp.data.data(), resp.data.size());
}
