#include "pinout.h"
#include "sleep.h"
#include "measure.h"

/** @file */

/// Persistent variables for measurement
struct measure_stack m1, m2;

struct pt schedule_thd, ///< Control structure for schedule() thread
measure2_thd, ///< Control structure for measure() thread
baseline2_thd, ///< Control structure for baseline() thread
delta2_thd; ///< Control structure for delta() thread


/** One-time initialization

This function is called when the microcontroller first starts up or is reset. It's good for things that should only happen once. You do not need to call it yourself.
It initializes some hardware, puts the protothreads in a known state, and begins the measurement cycle (which starts with sleep)
*/
void setup() {
  // Configure the external ADCs
  m2.addr = 5;
  m2.treeID = 4;
  // Initialize the hardware
  hardware_init();
  // Check that we have an RTC
  if (! rtc_ds.begin()) {
    Serial.println("Couldn't find RTC");
  }
  // If this is a new RTC, set the time
  if (rtc_ds.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // Set the RTC to the date & time this sketch was compiled
    rtc_ds.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  //Initialize the thread control structures
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
  Serial.print("Awoke at ");
  Serial.println(rtc_ds.now().text());
  static char rendezvous;
  /* Calculate the baseline temperatures */
  // Initialize baseline threads
  PT_INIT(&baseline2_thd);
  // Wait for threads to complete
  rendezvous = 1; // We have two copies running
  while(rendezvous){
    baseline(&baseline2_thd, m2, rendezvous);
    PT_YIELD(pt);
  }
  
  // Turn on the heater
  digitalWrite(HEATER, HIGH);
  Serial.print("Heater On at ");
  Serial.println(rtc_ds.now().text());
  PT_TIMER_DELAY(pt,3000); //< Heater is on for 3 seconds
  digitalWrite(HEATER, LOW); //< Turn off the heater
  Serial.print("Heater Off at ");
  Serial.println(rtc_ds.now().text());
  PT_TIMER_DELAY(pt,100*1000);  //< Wait for heat to propagate
  Serial.println("Temperature probably reached plateau");
  
  /* Calculate the sapflow, send over LoRa */
  // Initialize the delta threads
  PT_INIT(&delta2_thd);
  // Wait for threads to complete
  rendezvous = 1; // we have two copies running
  while(rendezvous){
    delta(&delta2_thd, m2, rendezvous);
    PT_YIELD(pt);
  }
  
  Serial.println("Finished logging");
  sleep_cycle(5);  //<Sleep until the next multiple of 5 minutes
  PT_RESTART(pt); //< Loop back to the beginning
  PT_END(pt);
}

/** Implicit loop

This function is called inside a hidden loop in the Arduino framework.
We're using it for protothread scheduling. All the real work happens inside the protothreads.
*/
void loop() {
  measure(&measure2_thd, m2);  //< Actually performs measurement.
  schedule(&schedule_thd); //< Dictates the timing of calculations

}
