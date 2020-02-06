// Feather9x_TX
// Designed to work with  Feather9x_RX

#include "lora.h"

void setup() 
{
  Serial.begin(115200);
  while (!Serial) {
    delay(1);
  }

  delay(100);

  Serial.println("Feather LoRa TX Test!");

  lora_init();
}


void loop()
{
  delay(5000); // Wait 1 second between transmits, could also 'sleep' here!
  
  build_msg(11.3, 7.8, 27.5, "5:01");
  send_msg();

}
