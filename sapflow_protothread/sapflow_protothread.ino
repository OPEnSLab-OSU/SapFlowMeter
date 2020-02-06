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
}

void loop() {
  //Check each thread by priority
  schedule();
  measure();
  sample_timer();
}
