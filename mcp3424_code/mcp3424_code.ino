#include "Sapflow_ADC.h"

struct temperature baseline;
uint8_t addr = 0x6E;
struct temperature c;

struct pt * pt;

void setup() {
  Serial.begin(115200);
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  digitalWrite(6, HIGH);
  get_temp(pt, addr, &baseline);
}

void loop() {
  get_temp(pt, addr, &c);
  c.upper = (c.upper - baseline.upper) * 1000;
  c.lower = (c.lower - baseline.lower) * 1000;
  c.heater = (c.heater - baseline.heater) * 1000;
  Serial.print("Upper:");
  Serial.print(c.upper);
  Serial.print(",Lower:");
  Serial.print(c.lower);
  Serial.print(",Heater:");
  Serial.println(c.heater);
}
