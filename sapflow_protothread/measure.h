#pragma once

#include "pinout.h"
#include "sleep.h"
#include "sd_log.h"
#include "lora.h"
#include <Wire.h>
#include "FeatherFault.h"

/** @file */

/** Stores a tuple of temperature values

Temperature values from the three probes
All three values have the same timestamp and are generally manipulated together to find the sap flow. (The heater probe temperature isn't part of the sap flow calculation but it can be used for monitoring and fault detection)
*/
struct temperature{
  double upper;  ///< Temperature (Celcius) at the upper probe
  double lower;  ///< Temperature (Celcius) at the lower probe
  double heater; ///< Temperature (Celcius) at the heater probe
};

/** Holds the persistent variables for a particular tree
 */
struct measure_stack{
  struct temperature latest; ///< The most recent temperature reading, measured by the measure() protothread
  struct temperature reference; ///< The baseline temperature reading, computed by the baseline() protothread
  int i; ///< variable used for counting iterations
  double flow; ///< Calculated sap flow
  double maxtemp; ///< Maximum temperature recorded this cycle
  struct pt_sem sem; ///< Semaphore for signaling new data
  int32_t raw[3]; ///< Stores raw readout from ADC
  uint8_t addr;   ///< I2C address of this ADC
  struct pt child; ///< Control structure for mcp3424_measure()
  int32_t treeID; ///< Identification number of this tree
};

/** Captures a measurement from the three probes.

This is a protothread that reads the temperature from three RTD amplifiers connected to the probes in the tree. It stores the result in the struct paramter "latest" and logs to the SD card.

@param pt A pointer to the protothread control structure. The default parameter is correct. Don't forget to initialize the control structure in setup().
@param m Persistent storage for this thread, like a private stack.
@returns the status of the protothread (Waiting, yeilded, exited, or ended)
*/
int measure(struct pt *pt, struct measure_stack &m);

/** Calculates baseline temperature.

This is a protothread that averages 10 samples of data to determine the "initial" or "baseline" temperature of the tree. It should be used before the heater is turned on.

@param pt A pointer to the protothread control structure. Don't forget to initialize the control structure in setup().
@param m Persistent storage for this thread, like a private stack.
@param rdv Rendezvous to ensure all baseline() instances stop at the same point
@returns the status of the protothread (Waiting, yeilded, exited, or ended)
*/
int baseline(struct pt *pt, struct measure_stack &m, char &rdv);

/** Calculates temperature delta and sapflow.

This is a protothread that calculates the sap flow by averaging measurements over 40 seconds.
It also calls other functions to get the weight, package the data, log to an SD card, and send the information over LoRa.

@param pt A pointer to the protothread control structure.
@param m Persistent storage for this thread, like a private stack.
@param rdv Rendezvous to ensure all delta() instances stop at the same point
@returns the status of the protothread (Waiting, yeilded, exited, or ended)
*/
int delta(struct pt *pt, struct measure_stack &m, char &rdv);

/** Converts raw measurement into degrees Celcius

@param raw The value measured by the ADC
@returns temperature in Celcius
*/
double rtd_calc(int32_t raw);

/** Measures the value of a single ADC channel

@param pt A pointer to the protothread control structure
@param addr The 7-bit I2C address of the ADC. If you choose, you can enter only the last three bits (0-7).
@param channel The measurement channel. Valid inputs are 1-4.
@param result The raw 18-bit signed ADC value. Result is bit-extended to 32 bits.
@returns the status of the protothread (Waiting, yeilded, exited, or ended)
*/
int mcp3424_measure(struct pt * pt, uint8_t addr, uint8_t channel, int32_t &result);
