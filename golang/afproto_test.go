package afproto

import (
	"testing"
	"encoding/hex"
)

func TestAfproto(t *testing.T) {
	if testing.Short() {
		return
	}

	var afp AfprotoFrame
	var msg = []byte("He\x85\x59\x85llo!")
	var test_buf []byte
	var rx_buf []byte


	test_buf = afp.Serialize(msg)

	/* Test length of test_buf */
	if (len(test_buf) != 16) {
		t.Log(len(test_buf))
		t.Log(test_buf)
		t.Log(hex.Dump(test_buf))
		t.Log(string(test_buf))
		t.Fail()
	}

	rx_buf = afp.Extract(test_buf)

	/* Did extraction fail? */
	if (rx_buf == nil) {
		t.Error("Extract Failure")
	}

	if (len(rx_buf) != len(msg)) {
		t.Log(len(rx_buf))
		t.Log(rx_buf)
		t.Log(hex.Dump(rx_buf))
		t.Log(string(rx_buf))
		t.Fail()
	}

	/* create a failure */
	rx_buf = afp.Extract([]byte("\x33\x33\x33\x33\x33\x33\x33"))
	if (rx_buf != nil) {
		t.Log(rx_buf)
		t.Log(hex.Dump(rx_buf))
		t.Error("nil expected, incorrect return value for malformed packet")
	}

	/* create another failure */
	rx_buf = afp.Extract([]byte(""))
	if (rx_buf != nil) {
		t.Log(rx_buf)
		t.Log(hex.Dump(rx_buf))
		t.Error("nil expected, incorrect return value for malformed packet")
	}

	/* Extract() buffer length assumption bug */
	rx_buf = afp.Extract([]byte("\xA3"))
	if (rx_buf != nil) {
		t.Error("nil expected, incorrect return for malformed packet")
	}
}

