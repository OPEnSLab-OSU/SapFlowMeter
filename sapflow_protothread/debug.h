#pragma once

#include <string.h> // for strncpy
#include <Arduino.h>
#include "pinout.h"

/** @file */

/** Used for debugging

This class allows us to find where the program halted by saving the line
number and function name into non-volatile memory.
*/
class FunctionMarker{
public:
  /// Initialize the watchdog at the period given.
  void init(int period);
  /// Record the current line number and function name. Also feeds the watchdog
  void set(int l, const char * str);
  /// Prints the most recently recorded value
  void print(void);
  /// Actually write the recorded value to flash.
  void write(void);
  /// Read the recorded value from flash
  bool read(void);
  /// Pause the watchdog. You'll want to do this before sleeping.
  void pause(void);
  /// Re-enable the watchdog. You should do this right after waking from sleep
  void resume(void);
private:
  char buffer[100];
  int line2;
  int period; // timeout period in ms
};

/** Watchdog for SAMD21

This class is borrowed from Adafruits SleepyDog library.
*/
class WatchdogSAMD {
public:
    WatchdogSAMD():
      _initialized(false)
    {}

    /** Enable the watchdog timer to reset the machine if it hangs
     * 
     * @param maxPeriodMS The desired millisecond value. Rounds up to the nearest power of 2. Possible values are 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, and 8192.
     * @returns The closest valid value
     */
    int enable(int PeriodMS);
    /// Reset or 'kick' the watchdog timer to prevent a reset of the device.
    void reset();
    /// Find out the cause of the last reset - see datasheet for bitmask
    uint8_t resetCause();
    /// Completely disable the watchdog timer.
    void disable();
private:
    void _initialize_wdt();
    bool _initialized;
};

/** Singleton of our debug class */
static class FunctionMarker halt_location;

/** This macro records the line number and function name

We use preprocessor directives to get the line number and function name.
This macro should be used everywhere you think the program might
possibly halt or crash.
*/
#define MARK() halt_location.set(__LINE__,__PRETTY_FUNCTION__)
