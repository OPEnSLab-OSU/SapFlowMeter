#pragma once

#include "pinout.h"
#include <string.h> // for memmove
#include <Arduino.h> // for Serial1

/** @file */

/** Stores the text recceived from the scale */
static char weight_buf[20];

/** Gets the weight from the scale

This function is intended for the OHAUS Defender 3000 scale.
It communicates over RS232 using 5V and -5V voltage levels, which will fry your microcontroller if connected directly. You will need an RS232 line driver.
This function prevents buffer overruns, but no data validation is performed.

@param buffer The string to store the data into (Default value is ok)
@param len The length of the buffer. (Default value is ok)
@returns The same buffer that was passed in, containing text read from the scale. */
char * read_weight( char * buffer = weight_buf, int len=20 );
