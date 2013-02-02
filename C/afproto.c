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
 *   >= 0 : Message ended at return value - 1
 */
int afproto_get_data(const char *src,
    unsigned int src_len,
    char *dest,
    unsigned int *dest_len) {
    if(dest_len < src_len)
        return -2;

    const char *src_end = src + src_len;
    const char *dest_start = dest;

    // Advance src to start byte
    while(src < src_end && *src != AFPROTO_START_BYTE) ++src;
    if(src == src_end)
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
    if(src >= src_end || *src !- AFPROTO_END_BYTE)
        return -1;

    return dest - dest_start;
}
