#include "pinout.h"
#include "schedule.h"
#include "measure.h"

/** @file */

/** One-time initialization

This function is called when the microcontroller first starts up or is reset. It's good for things that should only happen once. You do not need to call it yourself.
It initializes some hardware, puts the protothreads in a known state, and begins the measurement cycle (which starts with sleep)
*/
void setup() {
  pinMode(HEATER, OUTPUT);
  digitalWrite(HEATER, LOW);
  Serial.begin(115200);
//  while(!Serial);
  Serial.println("Serial connected");
  halt_location.init(2000);
  //Initialize the Threads
  PT_INIT(&sched_thd);
  PT_INIT(&measure_thd);
  PT_INIT(&sample_timer_thd);
  PT_INIT(&baseline_thd);
  PT_INIT(&delta_thd);
  sleep = false;
  
  pinMode(EN_3v3, OUTPUT);
  pinMode(EN_5v, OUTPUT);
  pinMode(I2C_SCL, INPUT_PULLUP);
  pinMode(I2C_SDA, INPUT_PULLUP);
  pinMode(RFM95_CS, OUTPUT);
  digitalWrite(RFM95_CS, HIGH); // disable LoRa until we're ready to use
  digitalWrite(STATUS_LED, HIGH);
  digitalWrite(EN_3v3, LOW); 
  digitalWrite(EN_5v, HIGH);
  pinMode(SPI_SCK, OUTPUT);
  pinMode(SPI_MOSI, OUTPUT);
  pinMode(SD_CS, OUTPUT);
  sd.begin(SD_CS, SD_SCK_MHZ(1));
}

/** Implicit loop

This function is called inside a hidden loop in the Arduino framework.
We're using it for protothread scheduling. All the real work happens inside the protothreads.
*/
void loop() {
  //Check each thread
  measure();
  schedule();
  sample_timer();
}
