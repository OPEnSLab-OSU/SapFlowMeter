#pragma once

#include <OPEnS_RTC.h>
#include <LowPower.h>
#include "pinout.h"
#include "measure.h"
static struct pt sched_thd;

static RTC_DS3231 rtc_ds;

static bool sleep; // Global flag to prep for sleep

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
Produces unexpected bevahior for non-factors of 60 (7, 8, 9, 11, etc).
For example, if it's 5:39 and you select an interval of 15, the microcontroller will wake up at 5:45, since 3*15 = 45.
This function internally calls feather_sleep() to handle prep and resume from sleeping

@param interval The increment to sleep for.
*/
void sleep_cycle( int interval = 5 );

/** Controls general measurement schedule.

This is the schedule:
1. Measure temperature of tree (before heat is applied)
2. Apply heat pulse
3. Wait for the peak of the heat to reach the upper and lower probes.
4. Measure the sap flow
5. Sleep until next measurement cycle
*/
int schedule(struct pt *pt = &sched_thd);
