#include "afproto.h"
#include "crc8.h"

#ifdef TEST_GCC
#include <stdio.h>
#endif

uint8_t afproto_frame_check_crc(AfprotoFrame *frame) {
	return crc_8(frame->payload, frame->length - 4) == frame->crc;
}

uint8_t afproto_get_frame_at(const char *buffer,
                          uint8_t length,
                          AfprotoFrame *frame) {
	uint8_t buff_itr = 0;


	if(buffer[0] != AFPROTO_FRAME_START_BYTE)
		return 0;

	while(buffer[buff_itr] != AFPROTO_FRAME_END_BYTE) buff_itr++;

	frame->length = buff_itr + 1;
	frame->payload = buffer+2;
	frame->crc = buffer[frame->length-2];

	if(frame->length != buffer[1] || !afproto_frame_check_crc(frame))
		return 0;

	return 1;
}

uint8_t afproto_get_frame(const char *buffer,
                       uint8_t length,
                       AfprotoFrame *frame) {
	uint8_t buff_itr = 0;
	uint8_t got_frame = 0;

	// Frame minimum size
	if(length < 4)
		return 0;

	while(!got_frame) {
		while(buffer[buff_itr] != AFPROTO_FRAME_START_BYTE && buff_itr < (length - 4)) buff_itr++;
		if(buff_itr >= (length - 4))
			return 0;
		if(afproto_get_frame_at(&buffer[buff_itr], length - buff_itr, frame))
			got_frame = 1;
	}

	return got_frame;
}

void afproto_serialize_frame(char *buffer,
                             uint8_t offset,
                             AfprotoFrame *frame) {
	uint8_t len = frame->length-4;
	uint8_t i;
	buffer[offset++] = AFPROTO_FRAME_START_BYTE;
	buffer[offset++] = frame->length;
	for(i = 0;len;len--,i++)
		buffer[offset++] = frame->payload[i];
	buffer[offset++] = frame->crc;
	buffer[offset++] = AFPROTO_FRAME_END_BYTE;
}

void afproto_create_frame(const char *buffer,
                          uint8_t length,
                          AfprotoFrame *frame) {
	frame->length = length + 4;
	frame->payload = buffer;
	frame->crc = crc_8(buffer, length);
}

#if TEST_GCC
int main(int argc, char **argv) {
	char buff[256];
	uint8_t off = 0;
	int i;

	AfprotoFrame f;
	afproto_create_frame("Hello", 6, &f);
	afproto_serialize_frame(buff, 0, &f);
	for(i = 0;i < f.length;++i)
		printf("%x ", (uint8_t)buff[i]);
	printf("\n");
	afproto_get_frame(buff, 255, &f);
	printf("%u (%u): %s\n", f.length-3, f.crc, f.payload);
}
#endif
