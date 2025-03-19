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

    #include <buffer.h>

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
    void do_request(std::vector<std::string> &cmd, Buffer &out);

    void response_begin(Buffer &out, size_t *header);
    size_t response_size(Buffer &out, size_t header);
    void response_end(Buffer &out, size_t header);

#endif /* !RESPONSE_H */
