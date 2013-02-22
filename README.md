Afproto
-------

A framing protocol optimized for embedded communication


## Repository Layout

Each directory contains source code for parsing the protocol in a specific
language and or implementation.


## Protocol

The protocol is a simplified version of RFC 1662.

Each frame begins and ends with byte 0x7E, with the data immediately following
the start byte.

The escape byte is 0x7D. Whenever 0x7E or 0x7D occur in the message that byte
is prefixed by the escape byte. That byte is then x-or'd with 0x20. As an
example, 0x7E in a message would become 0x7E5E.

Immediately following the data is a 16bit CRC (xmodem). The CRC checks the
serialized data (including escape characters).

Immediately following the CRC is the end byte.

A depiction of a frame:
<pre>
| Start Byte | Data | CRC16 | End Byte |
</pre>
