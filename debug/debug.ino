#include "debug.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  if(halt_location.read()){
    halt_location.print();
    halt_location.line++;
  } else {
    MARK();
  }
  halt_location.write();
}

void loop() {
  // put your main code here, to run repeatedly:
  halt_location.print();
  delay(1000);
}
