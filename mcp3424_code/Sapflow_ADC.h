/*****************************************************************
* Sapflow_ADC.h
* Josh Barksdale
* March 2020
* This library contains a single function which reads the
* temperature in Celsius from 3 different RTDs using an MCP3424
* ADC for the SapFlow project. It uses protothreads. It is based
* on the MCP342x library by B@tto.
******************************************************************/
#ifndef SAPADC_H
#define SAPADC_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define PT_USE_TIMER
#define PT_USE_SEM
#include <pt.h>
#include <Wire.h>

/** Struct to hold data for the get_temp protothread
It serves as a private stack for the protothread */
struct get_temp_t{
  struct pt pt; ///< Control structure for this protothread
  int32_t raw[3]; ///< Stores raw readout from ADC
  double deg_c[3];///< Stores interpolated temperature
  uint8_t addr;   ///< I2C address of this ADC
  struct pt child; ///< Control structure for mcp3424_measure()
  struct pt_sem sem; ///< Synchronizes producer and consumer
};
  

/// Bitmasks for the MCP3424 status/control register
enum register_mask{
  RDY=1<<7,  ///< result ready
  CHAN=3<<5, ///< Channel 0-3
  CON=1<<4,  ///< Continuous/One-Shot
  DEPTH=3<<2,  ///< Bit depth 12-18
  PGA=3<<0,   ///< Gain of 1-8
};

/** Gets the probe temperatures.

Samples once per second
*/
int get_temp(struct get_temp_t &s);

/// Converts raw measurement into degrees Celcius
double rtd_calc(int32_t raw);

/// Measures the value of a single ADC channel
int mcp3424_measure(struct pt * pt, uint8_t addr, uint8_t channel, int32_t &result);

#endif
