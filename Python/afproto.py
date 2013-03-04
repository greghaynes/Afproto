'''
Encode and decode afproto frames. For more information on the protocol see
https://github.com/greghaynes/Afproto/
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
    Returns a tuple of (data, extra_data). The data is data which was decoded
    from the passed frame, the extra_data is data that was not considered for
    parsing (and should probably be sent in a subsequent call).

    If no valid frame was found, data is None

    extra will always be a string (empty string if all data was considered).
    '''
    start_ndx = raw_frame.find(START_BYTE)
    if start_ndx == -1:
        return (None, '')

    end_ndx = raw_frame.find(END_BYTE, start_ndx + 1)
    if end_ndx == -1:
        return (None, raw_frame[start_ndx:])

    contents = unescape_data(raw_frame[start_ndx+1:end_ndx])
    data = contents[:-2]

    sent_crc = struct.unpack('H', contents[-2:])[0]
    if sent_crc != crc16.crc16_buff(data):
        print 'invalid crc'
        return (None, raw_frame[end_ndx+1:])

    return (data, raw_frame[end_ndx+1:])


def afproto_frame_data(data):
    '''
    Returns a raw frame which contains the supplied data
    '''
    ret = START_BYTE
    crc = struct.pack('H', crc16.crc16_buff(data))
    ret += escape_data(data)
    ret += escape_data(crc)
    ret += END_BYTE
    return ret


def main():
    'Main method'
    resp = afproto_get_data(afproto_frame_data('test') + afproto_frame_data('othertest'))
    print resp, afproto_get_data(resp[1])


if __name__ == '__main__':
    main()
