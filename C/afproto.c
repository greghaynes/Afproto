#include "afproto.h"
#include "crc16.h"

/*
 * Args:
 *   src      : source raw data
 *   src_len  : length of raw data
 *   dest     : destination buffer, must be at least length of src buffer
 *   dest_len : length of data in destination buffer, unless error returned
 * Return values:
 *   -3   : Destination buffer too small
 *   -2   : Invalid CRC
 *   -1   : No valid message found
 *   >= 0 : Message ended at src + return value
 * If  an error is returned, dest_len is set to the amount of data which can
 * be (and should be) discarded from the start of the src buffer.
 */
int afproto_get_data(const char *src,
    unsigned int src_len,
    char *dest,
    unsigned int *dest_len) {
    const char *src_start = src;
    const char *src_end = src + src_len;
    const char *dest_start = dest;
    short crc_check = 0;

    // Advance src to start byte
    while(src < src_end && *src != AFPROTO_START_BYTE) ++src;
    // Set our error return val for dest_len
    *dest_len = src - src_start;
    if(src >= src_end)
        return -1;

    // Loop through the data
    ++src;
    int prev_escape = 0;
    char orig_char;
    while(src < src_end - 2) {
        if(prev_escape) {
            prev_escape = 0;
            orig_char = (*src) ^ 0x20;
            crc_check = crc16_floating(orig_char, crc_check);
            *(dest++) = orig_char;
        }
        else if (*src == AFPROTO_ESC_BYTE) {
                prev_escape = 1;
        }
        else {
            orig_char = *src;
            crc_check = crc16_floating(orig_char, crc_check);
            *(dest++) = orig_char;
        }
        ++src;
    }

    // Check that we actually hit the end
    if(src[2] != AFPROTO_END_BYTE)
        return -1;

    // Check CRC
    short crc = *((short*)src);
    src += 3;
    if(crc != crc_check) {
        *dest_len = src - src_start;
        return -2;
    }

    *dest_len = dest - dest_start;
    return (src - src_start) - 1;
}

int afproto_frame_data(const char *src,
    unsigned int src_len,
    char *dest,
    unsigned int *dest_len) {
    const char *src_end = src + src_len;
    const char *dest_start = dest;
    short crc = 0;

    *(dest++) = AFPROTO_START_BYTE;

    int prev_escape = 0;
    while(src < src_end) {
        if(prev_escape) {
            prev_escape = 0;
            crc = crc16_floating(*src, crc);
            *(dest++) = *(src) ^ 0x20;
        }
        else if (*src == AFPROTO_START_BYTE || *src == AFPROTO_ESC_BYTE) {
            prev_escape = 1;
            *(dest++) = AFPROTO_ESC_BYTE;
            continue;
        }
        else {
            crc = crc16_floating(*src, crc);
            *(dest++) = *src;
        }
        ++src;
    }

    // Set the CRC
    // Dummy code
    *((short*)dest) = crc;
    dest += 2;

    *(dest++) = AFPROTO_END_BYTE;
    *dest_len = dest - dest_start - 1;
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

    int ret;
    if((ret = afproto_frame_data(orig_msg, 256, buff, &write_len)) < 0)
        printf("Data framing error %d!\n", ret);
    printf("Frame is %d bytes long\n", write_len);
    if((ret = afproto_get_data(buff, write_len, buff, &write_len)) < 0)
        printf("Get data error %d!\n", ret);

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
