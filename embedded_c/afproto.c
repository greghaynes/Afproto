#include "afproto.h"
#include "crc8.h"

#ifdef TEST_GCC
#include <stdio.h>
#endif

void afproto_unescape_buffer(const char *src, uint8_t len, char *dest) {
	uint8_t i;
	for(i = 0;i < len;++i) {
		if(src[i] == AFPROTO_FRAME_ESCAPE_BYTE && i < (len-1))
			++i;
		*(dest++) = src[i];
	}
}

uint8_t afproto_frame_crc_is_valid(AfprotoFrame *frame) {
	return crc_8(frame->payload, frame->length - 4) == frame->crc;
}

uint8_t afproto_get_payload(const char *buffer,
                            uint8_t length,
                            char *payload) {
	uint8_t ndx = 0;
	uint8_t start_ndx;
	uint8_t crc = 0;
	uint8_t length;

	// Find frame start
	while(ndx < length && buffer[ndx] != AFPROTO_FRAME_START_BYTE) {
		if(buffer[ndx] == AFPROTO_FRAME_ESCAPE_BYTE) ndx+=2;
		else ++ndx;
	} 

	// Set start ndx or return if not found
	if(buffer[ndx] != AFPROTO_FRAME_START_BYTE) return 0; 
	else start_ndx = ndx;

	++ndx;
	length = buffer[ndx];

	// Copy payload, gen crc, and unescape
	uint8_t end_ndx = length + ndx;
	uint8_t payload_ndx = 0;
	for(;ndx < end_ndx;++ndx) {
		if(buffer[ndx] == AFPROTO_FRAME_ESCAPE_BYTE) {
			crc = crc_8_update(crc, buffer[ndx]);
			++ndx;
			if(ndx == end_ndx)
				break;
		}
		crc = crc_8_update(crc, buffer[ndx]);
		payload[payload_ndx++] = buffer[ndx];
	}

	// Check crc and return
	if(crc != buffer[start_ndx+2]) return 0;
	else return ndx;
}

void afproto_serialize_frame(char *buffer,
                             AfprotoFrame *frame) {
	uint8_t len = frame->length-3;
	uint8_t i;
	uint8_t offset = 0;

	buffer[offset++] = AFPROTO_FRAME_START_BYTE;
	buffer[offset++] = frame->length;
	for(i = 0;i<len;++i) buffer[offset++] = frame->payload[i];
	buffer[offset++] = frame->crc;
}

void afproto_create_frame(char *buffer,
                          uint8_t length,
                          AfprotoFrame *frame) {
	frame->length = length + 3;
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

	strcpy(buff, "\x05\x88\x34\xa3\x0a\x48\x65\x6c\x6c\x6f\x00\xc9\x5D\x50\x89");
	afproto_get_frame(buff, 15, &f);

	printf("%s\n", f.payload);
}
#endif
