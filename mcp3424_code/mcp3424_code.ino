#include "Sapflow_ADC.h"

void setup() {
  Serial.begin(115200);
}

uint8_t addr = 0x6E;
struct temperature c;
struct pt * pt;
void loop() {
  get_temp(pt, addr, &c);
  Serial.print("Upper:");
  Serial.print(c.upper);
  Serial.print(",Lower:");
  Serial.print(c.lower);
  Serial.print(",Heater:");
  Serial.println(c.heater);
}
