import serial
import afproto

def debug_msgs(s):
	buff = ''
	while True:
		buff += s.read()
		if buff[-1] == chr(afproto.end_byte) and buff[-2] != chr(afproto.escape_byte):
			payload, buff = afproto.extract_payload(buff)
			print payload

if __name__=='__main__':
	s = serial.Serial('/dev/ttyUSB0', 38400)
	debug_msgs(s)

