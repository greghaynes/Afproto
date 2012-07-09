
/* This small example runs an infinite loop that will read 2-byte
afproto payloads and print them as 16-bit unsigned ints on the
console. */

package main

import (
	"fmt"
	"afproto"
	"serial"
	"os"
	"os/signal"
	"log"
	"io" /* for io.EOF and nothing else ... */
	"time"

	/* Note that the following packages can be uncommented to use
	the fancier method of converting serial bytes into real
	types*/

	//"encoding/binary"
	//"bytes"
)

func main() {
	var temp uint16 = 0
	var afp afproto.AfprotoFrame
	var rx_payload_buf []byte
	var rx_payload_n int = 0;
	var interrupt = make(chan os.Signal, 1)
	var rx_buf []byte


	if (len(os.Args) < 2) {
		fmt.Println("Must pass serial device as argument!")
		fmt.Println("e.g. ./blah /dev/ttyACM0")
		return
	} 

	/* install a signal handler so we can leave the
	never-ending-loop */
	signal.Notify(interrupt)

	conf := &serial.Config{Name: os.Args[1], Baud: 9600}

	s, err := serial.OpenPort(conf)
	if (err != nil) {
		log.Fatal(err)
	}
	
	/* Create a read buffer */
	read_buf := make([]byte, 256)

	/* loop until we catch ctrl-c */
	for {
		select {
		case <- interrupt:
			fmt.Println("caught a signal")
			/* close the serial dev... */
			s.Close()
			os.Exit(1)
		default:
			n, err := s.Read(read_buf)
			
			//fmt.Println(n)

			/* This will actually throw EOFs as we can
			read faster than the serial port is
			writing. So ... we ignore EOF failures. */ 
			if (err != nil && err != io.EOF) {
				log.Fatal(err)
			}

			if (n > 0) {
				rx_buf = append(rx_buf, read_buf[0:n]...)
			} else {
				/* nap */
				time.Sleep(50.0 * time.Millisecond)
			}

			if (len(rx_buf) >= 6) {
				//fmt.Println(len(rx_buf))
				//fmt.Println(rx_buf[3])
				
				rx_payload_n, rx_payload_buf, err = afp.Extract(rx_buf)
				//rx_payload_buf = afp.Extract(rx_buf)
				if (err == nil) {
					/* Commented out bit is the
					nice way to convert the raw
					bytes to a type */
					//byt_buf := bytes.NewBuffer(rx_payload_buf)
					//binary.Read(byt_buf, binary.LittleEndian, &temp)
					temp = uint16(rx_payload_buf[0])
					temp |= (uint16(rx_payload_buf[1]) << 8)
					fmt.Println(temp)
					
					/* Remove the bytes we read from rx_buf */
					rx_buf = rx_buf[rx_payload_n:]
				} else {
					log.Println(err)
					//fmt.Println("Afproto malformed packet?!")
				}
			}
		}
	}
}
