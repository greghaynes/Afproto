#include "afproto.h"

uint8_t afproto_frame_is_valid(AfprotoFrame *frame) {
}

void afproto_get_frame(const char *buffer,
                       uint16_t length,
                       uint8_t *offset,
                       AfprotoFrame *frame) {
	uint8_t buff_itr = *offset;
	*offset = 0;

	// Frames are larger than two btyes
	if(length < 3)
		return;

	while(buff_itr < length) {
		if(buffer[buff_itr] == AFPROTO_FRAME_START_BYTE) {
			frame->length = buffer[buff_itr+1];

			// Check that frame can fit in remaining buffer
			if(frame->length > (length - buff_itr))
				continue;

			frame->crc = buffer[buff_itr+2];
			frame->payload = &buffer[buff_itr+3];

			if(afproto_frame_is_valid(frame)) {
				*offset = buff_itr + frame->length;
				return;
			}
		}
		++buff_itr;
	}
}

#if TEST_GCC
int main(int argc, char **argv) {
}
#endif
