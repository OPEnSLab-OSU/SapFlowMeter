#pragma once

#include <string.h> // for strncpy
#include <Arduino.h>
#include "pinout.h"

class FunctionMarker{
public:
  void init(int period);
  void set(int l, const char * str);
  void print(void);
  void write(void);
  bool read(void);
  void pause(void);
  void resume(void);
  void mark(void);
private:
  char buffer[100];
  int line2;
  int period; // timeout period in ms
};

class WatchdogSAMD {
public:
    WatchdogSAMD():
      _initialized(false)
    {}

    /** Enable the watchdog timer to reset the machine if it hangs
     * 
     * @param maxPeriodMS The desired millisecond value. Powers of 2 from 8 to 8k.
     * @returns The closest valid value
     */
    int enable(int maxPeriodMS = 0);
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


static class FunctionMarker halt_location;

#define MARK() halt_location.set(__LINE__,__PRETTY_FUNCTION__); \
halt_location.mark()
