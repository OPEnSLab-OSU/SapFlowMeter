#include "pinout.h"
#include "schedule.h"
#include "measure.h"

/** @file */

/// Persistent variables for first 

/** One-time initialization

This function is called when the microcontroller first starts up or is reset. It's good for things that should only happen once. You do not need to call it yourself.
It initializes some hardware, puts the protothreads in a known state, and begins the measurement cycle (which starts with sleep)
*/
void setup() {
  // Initialize the hardware
  hardware_init();
  //Initialize the Threads
  PT_INIT(&sched_thd);
  PT_INIT(&measure_thd);
  PT_INIT(&baseline_thd);
  PT_INIT(&delta_thd);
}

/** Implicit loop

This function is called inside a hidden loop in the Arduino framework.
We're using it for protothread scheduling. All the real work happens inside the protothreads.
*/
void loop() {
  measure(&measure_thd, );  //< Actually performs measurement.
  schedule(); //< Controls the sequence of actions in our measurement
}
