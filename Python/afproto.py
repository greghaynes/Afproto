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
    pass
