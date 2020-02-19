#include "debug.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial);
  halt_location.read();
  halt_location.print();
  Serial.println("Started watchdog example");
  MARK();
  halt_location.print();
  delay(500);
  int time = wdt.enable(3000);
  Serial.print("Watchdog timer set to ");
  Serial.println(time);
}

void loop() {
  for( auto i = 0; ; ++i ){
    Serial.println(i);
    delay(1000);
  }
}
