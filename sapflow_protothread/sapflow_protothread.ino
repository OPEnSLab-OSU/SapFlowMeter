#define PT_USE_TIMER
#define PT_USE_SEM

#include <pt.h>
#include "schedule.h"
#include "measure.h"

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Serial connected");
  //Initialize the Threads
  PT_INIT(&sched_thd);
  PT_INIT(&measure_thd);
  PT_INIT(&sample_timer_thd);
  PT_INIT(&baseline_thd);
  PT_INIT(&delta_thd);
  sleep = false;
  pinMode(EN_3v3, OUTPUT);
  digitalWrite(EN_3v3, LOW);
  pinMode(EN_5v, OUTPUT);
  digitalWrite(EN_5v, HIGH);
  pinMode(I2C_SCL, INPUT_PULLUP);
  pinMode(I2C_SDA, INPUT_PULLUP);
  schedule();
  schedule();
}

void loop() {
  //Check each thread
  measure();
  schedule();
  sample_timer();
}
