#include "debug.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial);
  halt_location.init();
  Serial.println("Started watchdog example");
  int time = wdt.enable(500);
  Serial.print("Watchdog timer set to ");
  Serial.println(time);
}

void loop() {
  MARK();
  delay(100);
  MARK();
  delay(200);
  MARK();
  delay(300);
  MARK();
  delay(400);
  MARK();
  delay(500);
  MARK();
  delay(600);
  MARK();
  delay(700);
  MARK();
  delay(800);
  MARK();
  delay(900);
  MARK();
  delay(1000);
  MARK();
  delay(1100);
}
