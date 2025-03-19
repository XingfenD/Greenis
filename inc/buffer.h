#ifndef BUFFER_H
#define BUFFER_H

    #ifdef __linux__

    #include <string>

    #endif /* __linux */

    #include <vector>

    typedef std::vector<uint8_t> Buffer;

    void buf_append(Buffer &buf, const uint8_t *data, size_t len);
    void buf_consume(Buffer &buf, size_t n);

    /* help functions for the serialization */
    void buf_append_u8(Buffer &buf, uint8_t data);
    void buf_append_u32(Buffer &buf, uint32_t data);
    void buf_append_i64(Buffer &buf, int64_t data);
    void buf_append_dbl(Buffer &buf, double data);

    /* append serialized data types to the back */
    void out_nil(Buffer &out);
    void out_str(Buffer &out, const char *s, size_t size);
    void out_int(Buffer &out, int64_t val);
    void out_dbl(Buffer &out, double val);
    void out_err(Buffer &out, uint32_t code, const std::string &msg);
    void out_arr(Buffer &out, uint32_t n);

#endif /* !BUFFER_H */
