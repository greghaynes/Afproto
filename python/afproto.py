from crc8 import Crc8

import struct

start_byte = 0x0d

class Frame(object):
	def __init__(self, payload, crc=-1, crcer=Crc8()):
		self.crc = crc
		self.payload = payload
		self.crcer = crcer
	def set_crc(self):
		self.crc = self.crcer.digest(self.payload)
	def raw(self, gen_crc=True):
		if self.crc < 0 or gen_crc:
			self.set_crc()
		return struct.pack("BBB%ds"%len(self.payload), start_byte, len(self.payload)+3, self.crc, self.payload)
	def from_raw(self, raw_data):
		self.p_len, self.crc, self.payload = struct.unpack("BBs", raw_data)
	def is_valid(self):
		try:
			return self.p_len == len(self.payload)+3 and self.crc == self.crcer.digest(self.payload)
		except AttributeError:
			return self.crc == self.crcer.digest(self.payload)

if __name__=='__main__':
	f = Frame('Hello\0')
	out = f.raw()
	for b in out:
		print "%x" % ord(b),

