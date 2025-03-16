#include <buffer.h>
#include <query.h>

/* Append data to the outgoing buffer */
void
buf_append(Buffer &buf, const uint8_t *data, size_t len) {
    buf.insert(buf.end(), data, data + len);
}

/* Consume data from the incoming buffer */
void buf_consume(Buffer &buf, size_t n) {
    buf.erase(buf.begin(), buf.begin() + n);
}

/* help functions for the serialization */
void buf_append_u8(Buffer &buf, uint8_t data) {
    buf.push_back(data);
}

void buf_append_u32(Buffer &buf, uint32_t data) {
    buf_append(buf, (const uint8_t *)&data, 4);
}

void buf_append_i64(Buffer &buf, int64_t data) {
    buf_append(buf, (const uint8_t *)&data, 8);
}

void buf_append_dbl(Buffer &buf, double data) {
    buf_append(buf, (const uint8_t *)&data, 8);
}

/* append serialized data types to the back */
void out_nil(Buffer &out) {
    buf_append_u8(out, TAG_NIL);
}

void out_str(Buffer &out, const char *s, size_t size) {
    buf_append_u8(out, TAG_STR);
    buf_append_u32(out, (uint32_t)size);
    buf_append(out, (const uint8_t *)s, size);
}

void out_int(Buffer &out, int64_t val) {
    buf_append_u8(out, TAG_INT);
    buf_append_i64(out, val);
}

void out_dbl(Buffer &out, double val) {
    buf_append_u8(out, TAG_DBL);
    buf_append_dbl(out, val);
}

void out_err(Buffer &out, uint32_t code, const std::string &msg) {
    buf_append_u8(out, TAG_ERR);
    buf_append_u32(out, code);
    buf_append_u32(out, (uint32_t)msg.size());
    buf_append(out, (const uint8_t *)msg.data(), msg.size());
}

void out_arr(Buffer &out, uint32_t n) {
    buf_append_u8(out, TAG_ARR);
    buf_append_u32(out, n);
}
