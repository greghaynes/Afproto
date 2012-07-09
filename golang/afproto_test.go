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
	var rx_num int = 0


	test_buf = afp.Serialize(msg)
	
	/* Test length of test_buf */
	if (len(test_buf) != 16) {
		t.Log(len(test_buf))
		t.Log(test_buf)
		t.Log(hex.Dump(test_buf))
		t.Log(string(test_buf))
		t.Fail()
	}

	rx_num, rx_buf, err := afp.Extract(test_buf)

	/* Did extraction fail? */
	if (err != nil) {
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
	rx_num, rx_buf, err = afp.Extract([]byte("\x33\x33\x33\x33\x33\x33\x33"))
	if (err == nil) {
		t.Log(rx_buf)
		t.Log(hex.Dump(rx_buf))
		t.Error("nil expected, incorrect return value for malformed packet")
	}

	/* create another failure */
	rx_num, rx_buf, err = afp.Extract([]byte(""))
	if (err == nil) {
		t.Log(rx_buf)
		t.Log(hex.Dump(rx_buf))
		t.Error("nil expected, incorrect return value for malformed packet")
	}

	/* for length assumption bug in Extract() */
	rx_num, rx_buf, err = afp.Extract([]byte("\xA3"))
	if (err == nil) {
		t.Log(rx_num)
		t.Error("Expected an error, got none")
	}

	/* rx_num test -- Keep track of how much of the RX buffer
	Extract has covered*/
	rx_num_test_buf := append([]byte("\x33\x33"), test_buf...)
	rx_num, rx_buf, err = afp.Extract(rx_num_test_buf)
	if (err == nil) {
		if (rx_num != (len(test_buf) + 2)) {
			t.Log("test buf len: ", len(test_buf))
			t.Error("rx_num fail", rx_num, len(test_buf) + 2)
		}
	}
}

