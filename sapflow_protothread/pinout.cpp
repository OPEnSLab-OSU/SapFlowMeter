#include "pinout.h"
#include "sd_log.h"
#include "sleep.h"

/// @file

void hardware_init(void){
  pinMode(HEATER, OUTPUT);
  digitalWrite(HEATER, LOW);
  USBDevice.attach();
  Serial.begin(115200);
  Serial.println("Serial connected");
  FeatherFault::StartWDT(FeatherFault::WDTTimeout::WDT_2S);
  pinMode(EN_3v3, OUTPUT);
  pinMode(EN_5v, OUTPUT);
  pinMode(I2C_SCL, INPUT_PULLUP);
  pinMode(I2C_SDA, INPUT_PULLUP);
  pinMode(RFM95_CS, OUTPUT);
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
  // If this is a new RTC, set the time
  if (rtc_ds.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // Set the RTC to the date & time this sketch was compiled
    rtc_ds.adjust(compile_time);
  } else if( (rtc_ds.now()-compile_time).totalseconds() < 0){
    cout<<"RTC is in the past. Updating from "<<rtc_ds.now().text()
    <<" to "<<compile_time.text()<<endl;
    rtc_ds.adjust(compile_time);
  }
  MARK;
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
