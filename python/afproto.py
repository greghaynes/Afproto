from crc8 import Crc8

import struct

start_byte = 0xA3
end_byte = 0x5D

class Frame(object):
	def __init__(self, payload='', crc=-1, crcer=Crc8()):
		self.crc = crc
		self.payload = payload
		self.crcer = crcer
	def set_crc(self):
		self.crc = self.crcer.digest(self.payload)
	def raw(self, gen_crc=True):
		if self.crc < 0 or gen_crc:
			self.set_crc()
		return struct.pack(">BB%dsBB"%len(self.payload), start_byte, len(self.payload)+4, self.payload, self.crc, end_byte)
	def from_raw(self, raw_data):
		self.p_start_byte, self.p_len, self.payload, self.crc, self.p_end_byte = struct.unpack(">BB%dsBB"%(len(raw_data)-4), raw_data)
	def is_valid(self):
		try:
			return self.p_len == len(self.payload)+4 and self.crc == self.crcer.digest(self.payload) and self.p_start_byte == start_byte and self.p_end_byte == end_byte
		except AttributeError:
			return self.crc == self.crcer.digest(self.payload)
	def raw_len(self):
		return self.payload + 4

def extract_frame_at(data):
	end_ndx = -1
	f = Frame()
	if data[0] != chr(start_byte):
		return f
	while not f.is_valid() and end_ndx < len(data):
		end_ndx = data.find(chr(end_byte), end_ndx+1)
		f.from_raw(data[:end_ndx+1])
	return f

def extract_next_frame(data):
	start_ndx = data.find(chr(start_byte))
	f = extract_frame_at(data[start_ndx:])
	while not f.is_valid():
		start_ndx = data.find(chr(start_byte), start_ndx+1)
		f = extract_frame_at(data[start_ndx:])
	return f

if __name__=='__main__':
	f = Frame('Hello\0')
	out = f.raw()
	for b in out:
		print "%x" % ord(b),
	print
	f = extract_next_frame('\x05\x88\x34\xa3\x0a\x48\x65\x6c\x6c\x6f\x00\xc9\x5D\x50\x89')
	print f.is_valid(), f.payload

