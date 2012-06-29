#include <Afproto>

Afproto afp;//initialize an instance of the class

void setup(){/*nothing to setup*/}

void loop(){
     char msg[] = "asdf";
     char rx_buffer[256];
     char tx_buffer[256];

     int len;

     afp.serialize(msg, tx_buffer);
     len = afp.extract(tx_buffer, rx_buffer);

     if (len != 4) {
         //error
     }
}
