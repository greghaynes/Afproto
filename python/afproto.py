from crc8 import Crc8

import struct

start_byte = 0xA3
escape_byte = 0x85
end_byte = 0x59

crcer = Crc8()

def serialize_payload(payload):
	out = struct.pack('BBB', start_byte, len(payload), crcer.digest(payload))
	for ch in payload:
		if ch == start_byte or ch == end_byte or ch == escape_byte:
			out += escape_byte
		out += ch
	out += chr(end_byte)
	return out

def extract_payload(data):
	# Find start_ndx
	start_ndx = 0
	while start_ndx >= 0:
		start_ndx = data.find(chr(start_byte), start_ndx)
		if start_ndx >= 0:
			if start_ndx > 0 and data[start_ndx-1] != chr(escape_byte):
				break
			elif start_ndx == 0:
				break
		elif start_ndx == -1:
			break
		start_ndx += 1

	# If no start found trash data
	if start_ndx == -1:
		return None, ''

	# Not a full message
	if len(data) < start_ndx + 4:
		return None, data[start_ndx:]
	length, crc = struct.unpack('BB', data[start_ndx+1:start_ndx+3])

	# Check end byte
	end_ndx = start_ndx + length + 3
	try:
		if data[end_ndx] != chr(end_byte) or data[end_ndx-1] == chr(escape_byte):
			print 'No end byte'
			return None, data[start_ndx+1:]
	except IndexError:
		return None, data[start_ndx:]

	payload = data[start_ndx+3:end_ndx]

	if crc == crcer.digest(payload):
		# unescape
		payload = payload.replace(chr(escape_byte) + chr(start_byte), chr(start_byte))
		payload = payload.replace(chr(escape_byte) + chr(end_byte), chr(end_byte))
		payload = payload.replace(chr(escape_byte) + chr(escape_byte), chr(escape_byte))
		return payload, data[end_ndx+1:]

	print 'No payload'
	return None, data[end_ndx+1:]

if __name__=='__main__':
	print extract_payload("\xa3\x05\xeb\x48\x65\x6c\x6c\x6f\x59")
	out = serialize_payload('Hello')
	for ch in out:
		print '%x' % ord(ch),
	print

