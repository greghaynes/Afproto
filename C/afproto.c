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
            *(dest++) = (*src) ^ 0x20;
        }
        else if (*src == AFPROTO_ESC_BYTE) {
                ++src;
                prev_escape = 1;
                continue;
        }
        else
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
    const char *src_end = src + src_len;
    const char *dest_start = dest;

    *(dest++) = AFPROTO_START_BYTE;

    int prev_escape = 0;
    while(src < src_end) {
        if(prev_escape) {
            prev_escape = 0;
            *(dest++) = *(src) ^ 0x20;
        }
        else if (*src == AFPROTO_START_BYTE || *src == AFPROTO_ESC_BYTE){
            prev_escape = 1;
            *(dest++) = AFPROTO_ESC_BYTE;
            continue;
        }
        else
            *(dest++) = *src;
        ++src;
    }

    // Set the CRC
    // Dummy code
    dest += 2;

    *(dest++) = AFPROTO_END_BYTE;
    *dest_len = dest - dest_start;
    return 0;
}

#ifdef AFPROTO_TEST
#include <stdio.h>

int main(int argc, char **argv) {
    char orig_msg[257];
    char buff[512];
    unsigned int write_len;

    int i;
    for(i = 0;i < 256;++i)
        orig_msg[i]=(char)i;
    orig_msg[257] = 0;

    afproto_frame_data(orig_msg, 256, buff, &write_len);
    afproto_get_data(buff, write_len, buff, &write_len);

    for(i = 0;i < 256;++i) {
        if(buff[i] != orig_msg[i]) {
            printf("Error, %d\n", i);
            return 1;
        }
    }

    if(write_len != 256) {
        printf("Length not properly detected, got %d\n", write_len);
        return 1;
    }
    printf("Win!\n");
    return 0;
}
#endif
