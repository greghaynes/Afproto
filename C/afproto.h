#ifndef AFPROTO_H
#define AFPROTO_H

#define AFPROTO_START_BYTE 0x7D
#define AFPROTO_ESC_BYTE 0x7E
#define AFPROTO_END_BYTE 0x7F

int afproto_get_data(const char *src,
    unsigned int src_len,
    char *dest,
    unsigned int *dest_len);

int afproto_frame_data(const char *src,
    unsigned int src_len,
    char *dest,
    unsigned int *dest_len);

#endif
