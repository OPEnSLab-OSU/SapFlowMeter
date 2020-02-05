#include <OPEnS_RTC.h>
#include <LowPower.h>
static struct pt sched_thd;

#define HEATER 11
#define ALARM_PIN 12

RTC_DS3231 rtc_ds;

void alarmISR() {
  // Reset the alarm.
  rtc_ds.armAlarm(1, false);

  // Disable this interrupt
  detachInterrupt(digitalPinToInterrupt(ALARM_PIN));
}

void feather_sleep( void ){
  while(!digitalRead(ALARM_PIN)){
    pinMode(ALARM_PIN, INPUT_PULLUP);
    Serial.print("Waiting on alarm pin...");
    delay(10);
  }
  // Low-level so we can wake from sleep
  // Maybe a synchronization issue?
  attachInterrupt(digitalPinToInterrupt(ALARM_PIN), alarmISR, LOW);
  attachInterrupt(digitalPinToInterrupt(ALARM_PIN), alarmISR, LOW);
  // Prep for sleep
  Serial.end();
  USBDevice.detach();
  // Disable SPI to save power
  pinMode(SPI_SCK, INPUT);
  pinMode(SPI_MOSI, INPUT);
  pinMode(SD_CS, INPUT);
  // Turn off power rails
  digitalWrite(EN_3v3, HIGH); 
  digitalWrite(EN_5v, LOW);
  digitalWrite(STATUS_LED, LOW);
  // Sleep
  LowPower.standby();

  // Prep to resume
  digitalWrite(STATUS_LED, HIGH);
  digitalWrite(EN_3v3, LOW); 
  digitalWrite(EN_5v, HIGH);
  pinMode(SPI_SCK, OUTPUT);
  pinMode(SPI_MOSI, OUTPUT);
  pinMode(SD_CS, OUTPUT);
  USBDevice.attach();
  Serial.begin(115200);
//    sd.begin(chipSelect, SD_SCK_MHZ(1));
}

// Sleep until the time is a round multiple of the minute inteval.
// Produces unexpected bevahior for non-factors of 60 (7, 8, 9, 11, etc)
void sleep_cycle( int interval = 5 ){
  Serial.println("Sleeping until nearest multiple of 5 minutes");
  DateTime t = rtc_ds.now();
  t = t + TimeSpan( interval * 60 );
  uint8_t minutes = interval*(t.minute()/interval);
  rtc_ds.setAlarm(ALM2_MATCH_MINUTES, minutes, 0, 0);
  Serial.print("Alarm set to ");
  t = rtc_ds.getAlarm(2);
  Serial.println(t.text());
  delay(1000);
  feather_sleep();
}

static int schedule(struct pt *pt)
{
  PT_BEGIN(pt);
  // RTC Timer settings here
  if (! rtc_ds.begin()) {
    Serial.println("Couldn't find RTC");
  }
  // This may end up causing a problem in practice - what if RTC loses power in field? Shouldn't happen with coin cell batt backup
  if (rtc_ds.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // Set the RTC to the date & time this sketch was compiled
    rtc_ds.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  while (1)
  {
    Serial.print("Awoke at ");
    Serial.println(rtc_ds.now().text());
    PT_TIMER_DELAY(pt,10000);
    digitalWrite(HEATER, HIGH);
    Serial.print("Heater On at ");
    Serial.println(rtc_ds.now().text());
    PT_TIMER_DELAY(pt,6000);
    digitalWrite(HEATER, LOW);
    Serial.print("Heater Off at ");
    Serial.println(rtc_ds.now().text());
    PT_TIMER_DELAY(pt,60000);
    Serial.println("Temperature probably reached plateau");
    PT_TIMER_DELAY(pt,40000);
    Serial.println("Finished logging");
  }
  PT_END(pt);
}
