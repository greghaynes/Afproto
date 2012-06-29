#ifndef AFPROTOARDUINO_H
#define AFPROTOARDUINO_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

extern "C" {
    #include <string.h>
    #include <stdint.h>
    #include "embedded_c/afproto.h"
}

class Afproto { 
public:
    Afproto(){};
    ~Afproto(){};
    uint8_t serialize(const char *str, char* dest) { return afproto_serialize_payload((unsigned char*)str, strlen(str), (unsigned char*)dest); }
    uint8_t extract(const char *str, char* dest);
};

#endif
