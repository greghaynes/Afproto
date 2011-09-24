#include "afproto.h"
#include "crc8.h"

uint8_t afproto_get_payload(const char *buffer,
                            uint8_t buff_length,
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
	for(;ndx < end_ndx && ndx < buff_length;++ndx) {
		if(buffer[ndx] == AFPROTO_FRAME_ESCAPE_BYTE) {
			crc = crc_8_update(crc, buffer[ndx]);
			++ndx;
			if(ndx == end_ndx)
				break;
		}
		crc = crc_8_update(crc, buffer[ndx]);
		payload[payload_ndx++] = buffer[ndx];
	}

	// Check end byte
	ndx++;
	if(buffer[ndx] != AFPROTO_FRAME_END_BYTE) return 0;

	// Check length
	if(ndx - start_ndx != length + 4) return 0;

	// Check crc
	if(crc != buffer[start_ndx+2]) return 0;

	return ndx;
}

uint8_t afproto_serialize_payload(const char *payload,
                                  uint8_t length,
                                  char *dest) {
	uint8_t crc = 0;
	uint8_t ndx = 3;
	uint8_t payload_ndx = 0;
	uint8_t i;
	dest[0] = AFPROTO_FRAME_START_BYTE;

	for(;payload_ndx<length;++payload_ndx) {
		if(payload[payload_ndx] == AFPROTO_FRAME_START_BYTE ||
		   payload[payload_ndx] == AFPROTO_FRAME_ESCAPE_BYTE ||
		   payload[payload_ndx] == AFPROTO_FRAME_END_BYTE) {
			crc = crc_8_update(crc, AFPROTO_FRAME_ESCAPE_BYTE);
			dest[ndx++] = AFPROTO_FRAME_ESCAPE_BYTE;
		}
		crc = crc_8_update(crc, payload[payload_ndx]);
		dest[ndx++] = payload[payload_ndx];
	}

	dest[1] = length;
	dest[2] = crc;
	dest[ndx] = AFPROTO_FRAME_END_BYTE;
}

#if TEST_GCC
int main(int argc, char **argv) {
	char buff[256];
	uint8_t off = 0;
	int i;

}
#endif
