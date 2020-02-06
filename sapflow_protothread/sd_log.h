#pragma once

#include <SPI.h>
#include <SdFat.h>
#include <sdios.h> //for ArduinoOutStream
#include "pinout.h"

static ArduinoOutStream cout(Serial);

static SdFat sd; // File system object.

/* Checks if the filename exists.
* If so, appends an integer.
* Returns the new filename it came up with.*/
String newfile( String fname, String suffix=".csv" );
