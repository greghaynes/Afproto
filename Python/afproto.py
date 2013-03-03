'''
Encode and decode afproto frames. For more information on the protocol see
http://github.com/greghaynes/Afproto/tree/v2
'''

import crc16
import struct

START_BYTE = '\x7D'
ESC_BYTE = '\x7E'
END_BYTE = '\x7D'

def unescape_data(data):
    '''
    Takes in a string and returns the unescaped data
    '''
    ret = ''
    prev_escape = False
    for char_ in data:
        if not prev_escape:
            if char_ == ESC_BYTE:
                prev_escape = True
            else:
                ret += char_
        else:
            ret += chr(ord(char_) ^ 0x20)
            prev_escape = False

    if prev_escape:
        raise ValueError('Invalid data, ends in escape byte')

    return ret


def escape_data(data):
    '''
    Takes in a string and returns an escaped version of the string
    '''
    ret = ''
    for char_ in data:
        if char_ in (START_BYTE, ESC_BYTE, END_BYTE):
            ret += 'ESC_BYTE' + chr(ord(char_) ^ 0x20)
        else:
            ret += char_
    return ret


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
    data = escape_data(data)
    crc = struct.pack('H', crc16.crc16_buff(data))
    ret += data
    ret += escape_data(crc)
    ret += END_BYTE
    return ret


def main():
    'Main method'
    for chr_ in afproto_frame_data('test'):
        print '%x' % ord(chr_),


if __name__ == '__main__':
    main()
