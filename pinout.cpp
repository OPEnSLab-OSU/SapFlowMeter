#include "pinout.h"
#include "sd_log.h"
#include "sleep.h"
#include "battery.h"

/// @file

//attempted to disable featherfault

void hardware_init(void){
  pinMode(HEATER, OUTPUT);
  digitalWrite(HEATER, LOW);
  USBDevice.attach();
  //Serial.begin(115200);
  Serial.println("Serial connected");
  //FeatherFault::StartWDT(FeatherFault::WDTTimeout::WDT_4S);
  pinMode(EN_3v3, OUTPUT);
  pinMode(EN_5v, OUTPUT);
  pinMode(I2C_SCL, INPUT_PULLUP);
  pinMode(I2C_SDA, INPUT_PULLUP);
  pinMode(RFM95_CS, OUTPUT);
  pinMode(KILL_SWITCH, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(KILL_SWITCH, HIGH); //Set kill switch high, when turned low, circuit will turn off.
  digitalWrite(RFM95_CS, HIGH); //< disable LoRa until we're ready to use
  digitalWrite(STATUS_LED, HIGH);
  digitalWrite(EN_3v3, LOW);
  digitalWrite(EN_5v, HIGH);
  pinMode(SPI_SCK, OUTPUT);
  pinMode(SPI_MOSI, OUTPUT);
  pinMode(SD_CS, OUTPUT);
  MARK;
  sd.begin(SD_CS, SD_SCK_MHZ(1));
  MARK;
  rtc_ds.begin();
  MARK;
  // Pacific Daylight Time is UTC-7
  plog::TimeSync(rtc_ds.now(), -7);
  PLOGD << "Time set";

  //First time set up the LTC2943 control register
  batteryInit();
}

void hardware_deinit(void){
  // Disable SPI to save power
  pinMode(SPI_SCK, INPUT);
  pinMode(SPI_MOSI, INPUT);
  pinMode(SD_CS, INPUT);
  // Turn off power rails
  digitalWrite(EN_3v3, HIGH);
  digitalWrite(EN_5v, LOW);
  digitalWrite(STATUS_LED, LOW);MARK;
  Serial.println("Sleeping");
  FeatherFault::StopWDT();
  // Prep for sleep
  Serial.end();
  USBDevice.detach();
}
