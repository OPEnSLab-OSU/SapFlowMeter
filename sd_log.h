#pragma once

#include <SPI.h>
#include <SdFat.h>
#include <sdios.h>
#include "pinout.h"
#include <Plog.h>

/** @file */

/** Allows use of streams to print to Serial via cout 
@returns an ArduinoOutStream. Please don't use the return value. */
static ArduinoOutStream cout(Serial);

static SdFat sd; ///< File system object.

/** Initialize the system log */
void syslog_init(void);

/** Warn if a value is outside the expected bounds
 */
bool inrange(int treeID, char * name, double val, double min, double max);
