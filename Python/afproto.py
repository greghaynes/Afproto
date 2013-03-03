'''
Encode and decode afproto frames. For more information on the protocol see
http://github.com/greghaynes/Afproto/tree/v2
'''

import crc16
import struct

START_BYTE = chr(0x7D)
ESC_BYTE = chr(0x7E)
END_BYTE = chr(0x7D)

def afproto_get_data(raw_frame):
    '''
    Returns a new raw frame which was not considered for parsing. The
    typical usage is to reset a read buffer to this returned data after
    every call
    '''
    start_ndx = 0
    end_ndx = 0
    guts_buff = ''
    try:
        prev_escape = False
        while raw_frame[start_ndx] != START_BYTE or prev_escape:
            if prev_escape:
                prev_escape = False
            if raw_frame[start_ndx] == ESC_BYTE:
                prev_escape = True
            start_ndx += 1

        end_ndx = start_ndx + 1
        while raw_frame[end_ndx] != END_BYTE or prev_escape:
            if prev_escape:
                prev_escape = False

            if raw_frame[end_ndx] == ESC_BYTE:
                prev_escape = True
            else:
                guts_buff += raw_frame[end_ndx]
            end_ndx += 1
    except IndexError:
        return raw_frame[start_ndx:]

    data = guts_buff[:-2]
    raw_crc = guts_buff[-2:]
    return data


def afproto_frame_data(data):
    '''
    Returns a raw frame which contains the supplied data
    '''
    ret = START_BYTE
    crc = 0
    for byte in data:
        if byte in (START_BYTE, ESC_BYTE):
            ret += ESC_BYTE
            crc = crc16.crc16_floating(ESC_BYTE, crc)
        ret += byte
        crc = crc16.crc16_floating(byte, crc)
    ret += struct.pack('H', crc16.crc16_buff(data))
    ret += END_BYTE
    return ret

if __name__ == '__main__':
    for ch in afproto_frame_data('test'):
        print '%x' % ord(ch),
