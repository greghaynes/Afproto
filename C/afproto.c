#include "afproto.h"

/*
 * Args:
 *   src      : source raw data
 *   src_len  : length of raw data
 *   dest     : destination buffer, must be at least length of src buffer
 *   dest_len : length of destination buffer
 * Return values:
 *   -3   : Destination buffer too small
 *   -2   : Invalid CRC
 *   -1   : No valid message found
 *   >= 0 : Message ended at src + return value
 */
int afproto_get_data(const char *src,
    unsigned int src_len,
    char *dest,
    unsigned int *dest_len) {
    const char *src_end = src + src_len;
    const char *dest_start = dest;

    // Advance src to start byte
    while(src < src_end && *src != AFPROTO_START_BYTE) ++src;
    if(src >= src_end)
        return -1;

    // Loop through the data
    ++src;
    int prev_escape = 0;
    while(src < src_end && *src != AFPROTO_START_BYTE) {
        if(prev_escape) {
            prev_escape = 0;
        }
        else if (*src == AFPROTO_ESC_BYTE) {
                ++src;
                prev_escape = 1;
                continue;
        }
        *(dest++) = *src;
        ++src;
    }

    // We hit src end during data loop
    if(src == src_end)
        return -1;

    // Check CRC
    // Dummy code
    src += 2;

    // Check end btye
    if(src >= src_end || *src != AFPROTO_END_BYTE)
        return -1;

    *dest_len = dest - dest_start;
    return src_end - src;
}

int afproto_frame_data(const char *src,
    unsigned int src_len,
    char *dest,
    unsigned int *dest_len) {
    const char *dest_end = dest + *dest_len;
    const char *src_end = src + src_len;
    const char *dest_start = dest;

    if(dest < dest_end)
        *(dest++) = AFPROTO_START_BYTE;
    else
        return -1;

    int prev_escape = 0;
    while(dest < dest_end && src < src_end) {
        if(prev_escape) {
            prev_escape = 0;
        }
        else if (*src == AFPROTO_START_BYTE || *src == AFPROTO_ESC_BYTE){
            prev_escape = 1;
            *(dest++) = AFPROTO_ESC_BYTE;
            continue;
        }
        *(dest++) = *src;
    }

    if(dest >= dest_end)
        return -1;

    // Set the CRC
    // Dummy code
    dest += 2;

    if(dest >= dest_end)
        return -1;

    *(dest++) = AFPROTO_END_BYTE;
    *dest_len = dest - dest_start;
    return 0;
}

int main(int argc, char **argv) {
    return 0;
}
