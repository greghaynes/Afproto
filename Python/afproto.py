import crc16
import struct

start_byte = chr(0x7D)
esc_byte = chr(0x7E)
end_byte = chr(0x7D)

def afproto_get_data(raw_frame):
    '''
       Returns a new raw frame which was not considered for parsing. The
       typical usage is to reset a read buffer to this returned data after
       every call
    '''
    start_ndx = 0
    end_ndx = 0
    try:
        prev_escape = False
        while raw_frame[start_ndx] != start_byte or prev_escape:
            if prev_escape:
                prev_escape = False
            if raw_frame[start_ndx] == esc_byte:
                prev_escape = True
            start_ndx += 1
        end_ndx = start_ndx + 1
        while raw_frame[end_ndx] != end_byte or prev_escape:
            if prev_escape:
                prev_escape = False
            if raw_frame[end_ndx] == esc_byte:
                prev_escape = True
            end_ndx += 1
    except IndexError:
        return raw_frame[start_ndx:]

def afproto_frame_data(data):
    '''
       Returns a raw frame which contains the supplied data
    '''
    ret = start_byte
    crc = 0
    for byte in data:
        if byte in (start_byte, esc_byte):
            ret += esc_byte
            crc = crc16.crc16_floating(esc_byte, crc)
        ret += byte
        crc = crc16.crc16_floating(byte, crc)
    ret += struct.pack('H', crc16.crc16_buff(data))
    ret += end_byte
    return ret

if __name__ == '__main__':
    for ch in afproto_frame_data('test'):
        print '%x' % ord(ch),
