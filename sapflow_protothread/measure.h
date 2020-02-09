#pragma once

#include "pinout.h"
#include "schedule.h"
#include "sd_log.h"
#include "weight.h"
#include <Adafruit_MAX31865.h>

/** @file */
static bool sample_trigger; ///< global flag for synchronizing live data processing. Set by sample_timer()

#define Rnom 100.0 ///< Nominal RTD resistance in ohms
#define Rref 430.0 ///< Nominal reference resistor in ohms

/** Stores a tuple of temperature values

Temperature values from the three probes
All three values have the same timestamp and are generally manipulated together to find the sap flow. (The heater probe temperature isn't part of the sap flow calculation but it can be used for monitoring and fault detection)
*/
struct temperature{
  float upper;  ///< Temperature (Celcius) at the upper probe
  float lower;  ///< Temperature (Celcius) at the lower probe
  float heater; ///< Temperature (Celcius) at the heater probe
};

static struct temperature latest; ///< The most recent temperature reading, measured by the measure() protothread
static struct temperature reference; ///< The baseline temperature reading, computed by the baseline() protothread

static struct pt measure_thd; ///< Protothread control structure for measure()
static struct pt sample_timer_thd; ///< Protothread control structure for sample_timer()
static struct pt baseline_thd; ///< Protothread control structure for baseline()
static struct pt delta_thd; ///< Protothread control structure for delta()

/** Captures a measurement from the three probes.

This is a protothread that reads the temperature from three RTD amplifiers connected to the probes in the tree. It stores the result in the global variable "latest", and logs to the SD card.

@param pt A pointer to the protothread control structure. The default parameter is correct. Don't forget to initialize the control structure in setup().
@returns the status of the protothread (Waiting, yeilded, exited, or ended)
*/
int measure(struct pt *pt = &measure_thd);

/** Controls timing of the measurements.

This is a protothread that creates a pulse every second on the global variable sample_trigger in order to sychronize sample-based processing. Protothreads that sample or process sampled data can latch this signal using 
    PT_WAIT_UNTIL(pt, sample_trigger);
    PT_WAIT_WHILE(pt, sample_trigger);
to synchronize execution without impacting the sample frequency.

@param pt A pointer to the protothread control structure. The default parameter is correct. Don't forget to initialize the control structure in setup().
@returns the status of the protothread (Waiting, yeilded, exited, or ended)
*/
int sample_timer(struct pt *pt = &sample_timer_thd);

/** Calculates baseline temperature.

This is a protothread that averages 10 samples of data to determine the "initial" or "baseline" temperature of the tree. It should be used before the heater is turned on.

@param pt A pointer to the protothread control structure. The default parameter is correct. Don't forget to initialize the control structure in setup().
@returns the status of the protothread (Waiting, yeilded, exited, or ended)
*/
int baseline(struct pt *pt = &baseline_thd);

/** Calculates temperature delta and sapflow.

This is a protothread that calculates the sap flow by averaging measurements over 40 seconds.
It also calls other functions to get the weight, package the data, log to an SD card, and send the information over LoRa.

@param pt A pointer to the protothread control structure. The default parameter is correct. Don't forget to initialize the control structure in setup().
@returns the status of the protothread (Waiting, yeilded, exited, or ended)
*/
int delta(struct pt *pt = &delta_thd);
