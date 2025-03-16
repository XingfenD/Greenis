/**
 * @file ./inc/response.h
 * @brief Announcements request parsing, command handling, and response generation for a key-value store.
 * @author Fendy (xingfen.star@gmail.com)
 * @version 1.0
 * @date 2025-03-16
 * @copyright Copyright (c) 2025
 */

#ifndef RESPONSE_H
#define RESPONSE_H

    #include <stdint.h>

    #include <vector>

    #define K_MAX_ARGS ((size_t) (200 * 1000))


    /* Response::status */
    typedef enum {
        RES_OK = 0,
        RES_ERR = 1,    /* error */
        RES_NX = 2,     /* key not found */
    } ResponseStatus;

    /*
    * +--------+---------+
    * | status | data... |
    * +--------+---------+
    */
    struct Response {
        ResponseStatus status = RES_OK;
        std::vector<uint8_t> data;
    };

    /**
     * @brief Parse the request data into a list of strings.
     *
     * This function takes a byte array representing the request data, parses it according to the specified format,
     * and extracts a list of strings. It performs strict checks to ensure data integrity and safety limits.
     *
     * @param data Pointer to the request data byte array.
     * @param size Size of the request data in bytes.
     * @param out Reference to a vector where the parsed strings will be stored.
     * @return int32_t Returns 0 on success, -1 on failure (e.g., invalid format, safety limit exceeded, or trailing garbage).
     */
    int32_t parse_req(const uint8_t *data, size_t size, std::vector<std::string> &out);

    /**
     * @brief Execute the command specified in the request and generate a response.
     *
     * This function processes the command stored in the `cmd` vector and performs the corresponding action on the global key-value store.
     * It supports basic key-value operations such as `get`, `set`, and `del`, and handles unrecognized commands or invalid formats by setting the response status accordingly.
     *
     * @param cmd Reference to a vector containing the parsed command strings.
     * @param out Reference to a Response object where the result of the command execution will be stored.
     */
    void do_request(std::vector<std::string> &cmd, Response &out);

    /**
     * @brief Generate a response byte array based on the Response object.
     *
     * This function takes a Response object, calculates the total length of the response data, and constructs a byte array
     * containing the response length, status code, and data. The resulting byte array is stored in the `out` vector.
     *
     * @param resp Reference to a Response object containing the response data.
     * @param out Reference to a vector where the generated response byte array will be stored.
     */
    void make_response(const Response &resp, std::vector<uint8_t> &out);

#endif /* !RESPONSE_H */
