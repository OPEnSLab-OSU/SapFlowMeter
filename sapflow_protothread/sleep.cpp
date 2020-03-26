#include "sleep.h"

/// @file 

void alarmISR() {
  // Disable this interrupt
  detachInterrupt(digitalPinToInterrupt(ALARM_PIN));
}

void feather_sleep( void ){MARK;
  // Wait for the alarm to clear
  while(!digitalRead(ALARM_PIN)){
    pinMode(ALARM_PIN, INPUT_PULLUP);
    Serial.print("Waiting on alarm pin...");
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
// Produces unexpected bevahior for non-factors of 60 (7, 8, 9, 11, etc)
void sleep_cycle( int interval ){MARK;
  Serial.print("Sleeping until nearest multiple of ");
  Serial.print(interval);
  Serial.println(" minutes");MARK;
  DateTime t = rtc_ds.now();MARK;
  t = t + TimeSpan( interval * 60 );MARK;
  uint8_t minutes = interval*(t.minute()/interval);MARK;
  rtc_ds.setAlarm(ALM2_MATCH_MINUTES, minutes, 0, 0);MARK;
  Serial.print("Alarm set to ");MARK;
  t = rtc_ds.getAlarm(2);MARK;
  Serial.println(t.text());MARK;
  feather_sleep();MARK;
}
