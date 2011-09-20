#ifndef AFPROTO_H
#define AFPROTO_H

#if TEST_GCC
#include <stdint.h>
#endif

#define AFPROTO_FRAME_START_BYTE 0xA3
#define AFPROTO_FRAME_END_BYTE 0x5D

typedef struct AfprotoFrame {
	uint8_t length;
	uint8_t crc;
	const char *payload;
} AfprotoFrame;

uint8_t afproto_get_frame(const char *buffer,
                          uint8_t length,
                          AfprotoFrame *frame);

void afproto_serialize_frame(char *buffer,
                             uint8_t offset,
                             AfprotoFrame *frame);

void afproto_create_frame(const char *buffer,
                          uint8_t length,
                          AfprotoFrame *frame);

#endif
