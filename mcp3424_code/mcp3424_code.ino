#include "Sapflow_ADC.h"

struct get_temp_t gt;
struct pt graph_thd;

void setup() {
  Wire.begin();
  Serial.begin(115200);
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  digitalWrite(6, HIGH);
  PT_INIT(&gt.pt);
  PT_INIT(&graph_thd);
  gt.addr = 0x6E;
  PT_SEM_INIT(&gt.sem, 0);
}

int graph(struct pt * pt, struct pt_sem * sem){
  PT_BEGIN(pt);
  PT_SEM_WAIT(pt, sem);
  Serial.print("Upper:");
  Serial.print(gt.deg_c[0]);
  Serial.print(",Lower:");
  Serial.print(gt.deg_c[1]);
  Serial.print(",Heater:");
  Serial.print(gt.deg_c[2]);
  PT_RESTART(pt);
  PT_END(pt);
}

void loop() {
  PT_SCHEDULE(get_temp(gt));
  PT_SCHEDULE(graph(&graph_thd, &gt.sem));
}
