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
    pass

def afproto_frame_data(data):
    '''
       Returns a raw frame which contains the supplied data
    '''
    ret = start_byte
    ret += data
    ret += struct.pack('H', crc16.crc16_buff(data))
    ret += end_byte
    return ret

if __name__ == '__main__':
    for ch in afproto_frame_data('test'):
        print '%x' % ord(ch),
