#ifndef CRC16_H
#define CRC16_H

unsigned short crc16_buff(const void *buf, int len);
unsigned short crc16_floating(char next, unsigned short seed);

#endif
