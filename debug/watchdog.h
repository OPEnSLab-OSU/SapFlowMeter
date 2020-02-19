#ifndef WATCHDOGSAMD_H
#define WATCHDOGSAMD_H

#include <Arduino.h>

class WatchdogSAMD {
public:
    WatchdogSAMD():
      _initialized(false)
    {}

    /** Enable the watchdog timer to reset the machine if it hangs
     * 
     * @param maxPeriodMS The desired millisecond value. Powers of 2 from 8 to 8096.
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

#endif
