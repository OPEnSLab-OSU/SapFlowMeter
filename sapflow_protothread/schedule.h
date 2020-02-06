#pragma once

#include <OPEnS_RTC.h>
#include <LowPower.h>
#include "pinout.h"
#include "measure.h"
static struct pt sched_thd;

static RTC_DS3231 rtc_ds;

static bool sleep; // Global flag to prep for sleep

// Interrupt handler for RTC alarm
void alarmISR(void);

// Routine to put things away for sleep
void feather_sleep( void );

// Sleep until the time is a round multiple of the minute inteval.
// Produces unexpected bevahior for non-factors of 60 (7, 8, 9, 11, etc)
void sleep_cycle( int interval = 5 );

// Thread for main state machine
int schedule(struct pt *pt = &sched_thd);
