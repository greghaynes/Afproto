#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

extern "C" {
    #include <string.h>
    #include <stdint.h>
}
#include "Afproto.h"

uint8_t Afproto::extract(const char* str, char* dest) {
    uint8_t payload_len = 0;
    uint8_t ndx = 0;

    ndx = afproto_get_payload((unsigned char*)str, strlen(str), (unsigned char*)dest, &payload_len);

    /* Return payload length in real chars*/
    return(payload_len);
}

