#include "pinout.h"
#include "sleep.h"
#include "measure.h"
#include "sd_log.h"

/** @file */

const int thd_count = 2;
/// Persistent variables for measurement
struct measure_stack m1, m2;

struct pt schedule_thd, ///< Control structure for schedule() thread
measure1_thd, ///< Control structure for measure() thread
baseline1_thd, ///< Control structure for baseline() thread
delta1_thd, ///< Control structure for delta() thread
measure2_thd, ///< Control structure for measure() thread
baseline2_thd, ///< Control structure for baseline() thread
delta2_thd; ///< Control structure for delta() thread


/** One-time initialization

This function is called when the microcontroller first starts up or is reset. It's good for things that should only happen once. You do not need to call it yourself.
It initializes some hardware, puts the protothreads in a known state, and begins the measurement cycle (which starts with sleep)
*/
void setup() {
  // Configure the external ADCs I2C addresses
  m1.addr = 5;
  m2.addr = 6;
  // Configure the tree IDs
  // FIXME: Change these if deploying multiple dataloggers
  m1.treeID = 1;
  m2.treeID = 2;
  // Initialize the hardware
  hardware_init();
  // Initialize the logger
  syslog_init();
  //Initialize the thread control structures
  PT_INIT(&measure1_thd);
  PT_INIT(&measure2_thd);
  PT_INIT(&schedule_thd);
}

/** Controls the schedule of heating and sleeping

It waits on each instance of baseline() and delta(), ensuring they each complete before continuing to the next stage. This function is in the main .ino because you will need to modify it when adding additional probes. 

This is the schedule:
1. Measure temperature of tree (before heat is applied)
2. Apply heat pulse
3. Wait for the peak of the heat to reach the upper and lower probes.
4. Measure the sap flow
5. Sleep until next measurement cycle
@param pt A pointer to the protothread control structure.
@returns the status of the protothread (Waiting, yeilded, exited, or ended)
*/
int schedule(struct pt * pt){
  PT_BEGIN(pt);
  static char rendezvous;
  /* Calculate the baseline temperatures */
  // Initialize baseline threads
  PT_INIT(&baseline1_thd);
  PT_INIT(&baseline2_thd);
  // Wait for threads to complete
  rendezvous = thd_count;
  while(rendezvous){
    baseline(&baseline1_thd, m1, rendezvous);
    baseline(&baseline2_thd, m2, rendezvous);
    PT_YIELD(pt);
  }
  
  // Turn on the heater
  digitalWrite(HEATER, HIGH);
  PLOG_INFO << "Heater On";
  PT_TIMER_DELAY(pt,3000); //< Heater is on for 3 seconds
  digitalWrite(HEATER, LOW); //< Turn off the heater
  PLOG_INFO << "Heater Off";
  
  /* Calculate the sapflow, send over LoRa */
  // Initialize the delta threads
  PT_INIT(&delta1_thd);
  PT_INIT(&delta2_thd);
  // Wait for threads to complete
  rendezvous = thd_count;
  while(rendezvous){
    delta(&delta1_thd, m1, rendezvous);
    delta(&delta2_thd, m2, rendezvous);
    PT_YIELD(pt);
  }
  
  PLOG_VERBOSE << "Finished logging";
  // Avoid transmitting at the same time as another datalogger
  int offset = 5 * max(m1.treeID, m2.treeID);
  sleep_cycle(5, offset);  //<Sleep until the next multiple of 5 minutes
  PT_RESTART(pt); //< Loop back to the beginning
  PT_END(pt);
}

/** Implicit loop

This function is called inside a hidden loop in the Arduino framework.
We're using it for protothread scheduling. All the real work happens inside the protothreads.
*/
void loop() {
  //measure(&measure1_thd, m1);  //< Actually performs measurement.
  measure(&measure2_thd, m2);  //< Actually performs measurement.
  schedule(&schedule_thd); //< Dictates the timing of calculations

}
