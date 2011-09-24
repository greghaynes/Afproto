#ifndef AFPROTO_H
#define AFPROTO_H

#if TEST_GCC
#include <stdint.h>
#endif

#define AFPROTO_FRAME_START_BYTE 0xA3
#define AFPROTO_FRAME_END_BYTE 0x59
#define AFPROTO_FRAME_ESCAPE_BYTE 0x85

uint8_t afproto_get_payload(const unsigned char *buffer,
                            uint8_t length,
                            unsigned char *payload,
                            uint8_t *payload_length);

uint8_t afproto_serialize_payload(const unsigned char *payload,
                                  uint8_t length,
                                  unsigned char *dest);

#endif
