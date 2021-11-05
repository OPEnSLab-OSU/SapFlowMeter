#include "sleep.h"
#include "sd_log.h"

/// @file 

void alarmISR() {
  // Disable this interrupt
  detachInterrupt(digitalPinToInterrupt(ALARM_PIN));
}

void feather_sleep( void ){MARK;
  // Wait for the alarm to clear
  while(!digitalRead(ALARM_PIN)){
    pinMode(ALARM_PIN, INPUT_PULLUP);
    PLOG_DEBUG << "Waiting on alarm pin...";
    delay(10);
  }MARK;
  hardware_deinit();
  // Low-level so we can wake from sleep
  // We have to call this twice - maybe a synchronization issue?
  attachInterrupt(digitalPinToInterrupt(ALARM_PIN), alarmISR, LOW);
  attachInterrupt(digitalPinToInterrupt(ALARM_PIN), alarmISR, LOW);
  // Sleep
  LowPower.standby();
  // Resume
  hardware_init();
}

// Sleep until the time is a round multiple of the minute inteval.
void sleep_cycle( int interval, int offset ){
  MARK;
  PLOG_INFO<<"Sleeping until nearest multiple of "<<interval
  <<" minutes with "<<offset<<" seconds offset"<<endl;
  MARK;
  DateTime t = rtc_ds.now();MARK;
  interval *= 60;
  // Roll back to start of this interval using integer floor
  t = DateTime((t.unixtime()/interval)*interval);
  PLOG_VERBOSE<<"Rolled back to "<<t.text()<<endl;
  // Add interval and offset
  t = t + TimeSpan( interval + offset);MARK;
  rtc_ds.setAlarm(t);MARK;
  //PLOG_DEBUG<<"Alarm set to "<<rtc_ds.getAlarm(1).text()<<endl; //COMPILE
  MARK;
  feather_sleep();MARK;
}
