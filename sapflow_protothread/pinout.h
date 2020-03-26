#pragma once

/** @file */

/** Pin definitions for our project

This enum defines the board pinout.
It is similar to using DEFINE, but has the added benefit of
preventing address conflicts.
*/
enum pinout{
  EN_3v3 = 5, ///< Control pin for 3.3V power rail. Output, Active-low.
  EN_5v = 6, ///< Control pin for 5v Power rail. Output, Active-high.
  SD_CS = 10, ///< SPI chip select for SD card. Output, Active-low.
  HEATER = 17, ///< Control pin for heater switch. Output, Active-high.
  ALARM_PIN = 12, ///< Interrupt pin from RTC. Pull-up, Active-low.
  STATUS_LED = 13, ///< Built-in LED on feather. Active-high.
  I2C_SDA = 20, ///< I2C data pin. Pull-up.
  I2C_SCL = 21, ///< I2C clock pin. Pull-up.
  SPI_SCK = 24, ///< SPI clock pin. Output.
  SPI_MOSI = 23, ///< SPI data pin. Output.
  SPI_MISO = 22, ///< SPI data pin. Input.
  TX = 1, ///< Serial1 transmit, used for RS232. Output.
  RX = 0, ///< Serial1 receive, used for RS232. Input.
  RFM95_CS = 8, ///< SPI chip select used for LoRa. Output, Active-low.
  RFM95_RST = 4,///< Reset pin used for LoRa. Output. Active-low?
  RFM95_INT = 3,///< Interrupt pin used for LoRa. Input. Active-low?
};

// Other includes that everyone should have
#define PT_USE_TIMER
#define PT_USE_SEM
#include <pt.h>

#include <FeatherFault.h>

/** Initializes the hardware.
 *
 * Used at startup or when waking from sleep */
void hardware_init(void);

/** De-initializes hardware
 *
 * Used before sleeping to save power*/
void hardware_deinit(void);
