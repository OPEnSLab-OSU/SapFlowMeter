#pragma once

#include <OPEnS_RTC.h>
#include <LowPower.h>
#include "pinout.h"

static RTC_DS3231 rtc_ds; ///< Instance of our real-time clock
/** Compile time of our sketch
 *
 * Approximately the same as when the sketch was uploaded, but could
 * be off by tens of seconds
 */
static DateTime compile_time = DateTime(F(__DATE__), F(__TIME__));
/** @file */

/** Interrupt handler for RTC alarm

Called when the feather wakes up from sleep. Disables the RTC interrupt.
*/
void alarmISR(void);

/** Maximizes power savings during sleep.

##Steps for going to sleep:##
1. Attach interrupt to RTC pin
2. Disconnect from the SD card
3. Disconnect from Serial and USB
4. Turn off power rails and status LED
5. Begin deep sleep
##Steps for waking up:##
1. Attach USB and Serial
2. Turn on power rails and status LED
3. Open SD card

*/
void feather_sleep( void );

/** Sleep function for periodic sleeping.

Sleep until the time is a round multiple of the minute inteval.
For example, if it's 5:39 and you select an interval of 15, the microcontroller will wake up at 5:45, since 3*15 = 45.
This function internally calls feather_sleep() to handle prep and resume from sleeping

@param interval The increment to sleep for (in minutes)
@param offset Additional delay before waking up (in seconds). This is useful for staggering wakeup times to avoid LoRa packet collision
*/
void sleep_cycle( int interval = 5, int offset=0 );
