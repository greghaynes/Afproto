// Copyright (c) 2011 K Jonathan Harker <kjharke@cs.pdx.edu>
// This source code is released under the terms of the
// MIT license. Please see the file LICENSE for license details.

// This is a direct port of embedded_c/afproto.{c,h} into go

package afproto

const (
	StartByte  byte = 0xA3
	EndByte    byte = 0x59
	EscapeByte byte = 0x85
)

var crcTable = []byte{
	0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
	157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
	35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
	190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
	70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
	219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
	101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
	248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
	140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
	17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
	175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
	50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
	202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
	87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
	233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
	116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53}

type AfprotoFrame struct {
	crc    byte
	length byte
	stream []byte
}

func (self *AfprotoFrame) updateCrc(i byte) {
	self.crc = crcTable[self.crc^i]
}

func (self *AfprotoFrame) Serialize(input []byte) []byte {
	var buffer []byte

	self.stream = make([]byte, 0)
	self.length = 0
	self.crc = 0

	for _, thisByte := range input {
		//we might need an escape
		switch thisByte {
		case StartByte, EndByte, EscapeByte:
			//append an escape character
			buffer = append(buffer, EscapeByte)
			self.updateCrc(EscapeByte)
			fallthrough
		default:
			buffer = append(buffer, thisByte)
			self.updateCrc(thisByte)
		}
	}

	self.length = byte(len(buffer))

	//build the actual data stream
	self.stream = append(self.stream, StartByte, self.length, self.crc)
	self.stream = append(self.stream, buffer...)
	self.stream = append(self.stream, EndByte)

	return self.stream
}

func (self *AfprotoFrame) Extract(incoming []byte) []byte {
	var flag bool = false

	if (len(incoming) < 1) {
		return nil
	}

	//flag is true when we have found the start byte
	for !flag {
		switch incoming[0] {
		case StartByte:
			flag = true
		case EscapeByte:
			incoming = incoming[1:]
			fallthrough
		default:
			if (len(incoming) > 1) {
				incoming = incoming[1:]
			} else {
				/* reached end of incoming with no flags */
				return nil
			}
		}
	}

	/* incoming is now assumed to have at least 4 bytes in
	it. Fail if it doesn't. */
	if (len(incoming) < 4) {
		return nil
	}

	//metadata
	self.length = incoming[1]
	self.crc = incoming[2]
	self.stream = nil //ensure empty

	//flag is true if we have seen an escape char
	flag = false
	for _, thisByte := range incoming[3:] {
		if flag {
			self.stream = append(self.stream, thisByte)
			flag = false
		} else if thisByte == EscapeByte {
			flag = true
		} else if thisByte == EndByte {
			break
		} else {
			self.stream = append(self.stream, thisByte)
		}
	}

	return self.stream
}
