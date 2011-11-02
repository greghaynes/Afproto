package main

import (
    "afproto"
    "fmt"
)

func main() {
    var afp afproto.AfprotoFrame
    var test []byte

    test = append(test, []byte{1,2,3}...)

    test = afp.Serialize(test)

    test = afp.Extract(test)

    fmt.Println(test)
}
