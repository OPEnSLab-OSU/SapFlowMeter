#pragma once

// Pin definitions for our project
enum pinout{
  EN_3v3 = 5,
  EN_5v = 6,
  SD_CS = 10,
  HEATER = 11,
  ALARM_PIN = 12,
  STATUS_LED = 13,
  I2C_SDA = 20,
  I2C_SCL = 21,
  SPI_SCK = 24,
  SPI_MOSI = 23,
  SPI_MISO = 22,
  TX = 1, // serial1, used for RS232
  RX = 0, // serial1, used for RS232
//  UPPER_CS = A5, // Upper RTD
//  LOWER_CS = A4, // Lower RTD
//  HEATER_CS = A3, // Heater RTD
};

// Other includes that everyone should have
#define PT_USE_TIMER
#define PT_USE_SEM
#include <pt.h>
