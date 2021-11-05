#include <Wire.h>

/*!
| LTC2943 I2C Address Assignment    | Value |
| :-------------------------------- | :---: |
| LTC2943_I2C_ADDRESS               | 0xC8  |
*/

/*! @name LTC2943 I2C Address
@{ */

#define LTC2943_I2C_ADDRESS 0x64
#define LTC2943_I2C_ALERT_RESPONSE  0x0C
//! @}


/*!
| Name                                              | Value |
| :------------------------------------------------ | :---: |
| LTC2943_STATUS_REG                                | 0x00  |
| LTC2943_CONTROL_REG                               | 0x01  |
| LTC2943_ACCUM_CHARGE_MSB_REG                      | 0x02  |
| LTC2943_ACCUM_CHARGE_LSB_REG                      | 0x03  |
| LTC2943_CHARGE_THRESH_HIGH_MSB_REG                | 0x04  |
| LTC2943_CHARGE_THRESH_HIGH_LSB_REG                | 0x05  |
| LTC2943_CHARGE_THRESH_LOW_MSB_REG                 | 0x06  |
| LTC2943_CHARGE_THRESH_LOW_LSB_REG                 | 0x07  |
| LTC2943_VOLTAGE_MSB_REG                           | 0x08  |
| LTC2943_VOLTAGE_LSB_REG                           | 0x09  |
| LTC2943_VOLTAGE_THRESH_HIGH_MSB_REG               | 0x0A  |
| LTC2943_VOLTAGE_THRESH_HIGH_LSB_REG               | 0x0B  |
| LTC2943_VOLTAGE_THRESH_LOW_MSB_REG                | 0x0C  |
| LTC2943_VOLTAGE_THRESH_LOW_LSB_REG                | 0x0D  |
| LTC2943_CURRENT_MSB_REG                           | 0x0E  |
| LTC2943_CURRENT_LSB_REG                           | 0x0F  |
| LTC2943_CURRENT_THRESH_HIGH_MSB_REG               | 0x10  |
| LTC2943_CURRENT_THRESH_HIGH_LSB_REG               | 0x11  |
| LTC2943_CURRENT_THRESH_LOW_MSB_REG                | 0x12  |
| LTC2943_CURRENT_THRESH_LOW_LSB_REG                | 0x13  |
| LTC2943_TEMPERATURE_MSB_REG                       | 0x14  |
| LTC2943_TEMPERATURE_LSB_REG                       | 0x15  |
| LTC2943_TEMPERATURE_THRESH_HIGH_REG               | 0x16  |
| LTC2943_TEMPERATURE_THRESH_LOW_REG                | 0x17  |
*/

/*! @name Registers
@{ */
// Registers
#define LTC2943_STATUS_REG                          0x00
#define LTC2943_CONTROL_REG                         0x01
#define LTC2943_ACCUM_CHARGE_MSB_REG                0x02
#define LTC2943_ACCUM_CHARGE_LSB_REG                0x03
#define LTC2943_CHARGE_THRESH_HIGH_MSB_REG          0x04
#define LTC2943_CHARGE_THRESH_HIGH_LSB_REG          0x05
#define LTC2943_CHARGE_THRESH_LOW_MSB_REG           0x06
#define LTC2943_CHARGE_THRESH_LOW_LSB_REG           0x07
#define LTC2943_VOLTAGE_MSB_REG                     0x08
#define LTC2943_VOLTAGE_LSB_REG                     0x09
#define LTC2943_VOLTAGE_THRESH_HIGH_MSB_REG         0x0A
#define LTC2943_VOLTAGE_THRESH_HIGH_LSB_REG         0x0B
#define LTC2943_VOLTAGE_THRESH_LOW_MSB_REG          0x0C
#define LTC2943_VOLTAGE_THRESH_LOW_LSB_REG          0x0D
#define LTC2943_CURRENT_MSB_REG                     0x0E
#define LTC2943_CURRENT_LSB_REG                     0x0F
#define LTC2943_CURRENT_THRESH_HIGH_MSB_REG         0x10
#define LTC2943_CURRENT_THRESH_HIGH_LSB_REG         0x11
#define LTC2943_CURRENT_THRESH_LOW_MSB_REG          0x12
#define LTC2943_CURRENT_THRESH_LOW_LSB_REG          0x13
#define LTC2943_TEMPERATURE_MSB_REG                 0x14
#define LTC2943_TEMPERATURE_LSB_REG                 0x15
#define LTC2943_TEMPERATURE_THRESH_HIGH_REG         0x16
#define LTC2943_TEMPERATURE_THRESH_LOW_REG          0x17
//! @}

/*!
| Command Codes                                 | Value     |
| :-------------------------------------------- | :-------: |
| LTC2943_AUTOMATIC_MODE                        | 0xC0      |
| LTC2943_SCAN_MODE                             | 0x80      |
| LTC2943_MANUAL_MODE                           | 0x40      |
| LTC2943_SLEEP_MODE                            | 0x00      |
| LTC2943_PRESCALAR_M_1                         | 0x00      |
| LTC2943_PRESCALAR_M_4                         | 0x08      |
| LTC2943_PRESCALAR_M_16                        | 0x10      |
| LTC2943_PRESCALAR_M_64                        | 0x18      |
| LTC2943_PRESCALAR_M_256                       | 0x20      |
| LTC2943_PRESCALAR_M_1024                      | 0x28      |
| LTC2943_PRESCALAR_M_4096                      | 0x30      |
| LTC2943_PRESCALAR_M_4096_2                    | 0x31      |
| LTC2943_ALERT_MODE                            | 0x04      |
| LTC2943_CHARGE_COMPLETE_MODE                  | 0x02      |
| LTC2943_DISABLE_ALCC_PIN                      | 0x00      |
| LTC2943_SHUTDOWN_MODE                         | 0x01      |
*/

/*! @name Command Codes
@{ */
// Command Codes
#define LTC2943_AUTOMATIC_MODE                  0xC0
#define LTC2943_SCAN_MODE                       0x80
#define LTC2943_MANUAL_MODE                     0x40
#define LTC2943_SLEEP_MODE                      0x00

#define LTC2943_PRESCALAR_M_1                   0x00
#define LTC2943_PRESCALAR_M_4                   0x08
#define LTC2943_PRESCALAR_M_16                  0x10
#define LTC2943_PRESCALAR_M_64                  0x18
#define LTC2943_PRESCALAR_M_256                 0x20
#define LTC2943_PRESCALAR_M_1024                0x28
#define LTC2943_PRESCALAR_M_4096                0x30
#define LTC2943_PRESCALAR_M_4096_2              0x31

#define LTC2943_ALERT_MODE                      0x04
#define LTC2943_CHARGE_COMPLETE_MODE            0x02

#define LTC2943_DISABLE_ALCC_PIN                0x00
#define LTC2943_SHUTDOWN_MODE                   0x01

//! @}

/*!
| Conversion Constants                              |  Value   |
| :------------------------------------------------ | :------: |
| LTC2943_CHARGE_lsb                                | 0.34  mAh|
| LTC2943_VOLTAGE_lsb                               | 1.44   mV|
| LTC2943_CURRENT_lsb                               |  29.3  uV|
| LTC2943_TEMPERATURE_lsb                           | 0.25    C|
| LTC2943_FULLSCALE_VOLTAGE                         |  23.6   V|
| LTC2943_FULLSCALE_CURRENT                         |  60    mV|
| LTC2943_FULLSCALE_TEMPERATURE                     | 510     K|

*/
/*! @name Conversion Constants
@{ */
const float LTC2943_CHARGE_lsb = 0.34E-3;
const float LTC2943_VOLTAGE_lsb = 1.44E-3;
const float LTC2943_CURRENT_lsb = 29.3E-6;
const float LTC2943_TEMPERATURE_lsb = 0.25;
const float LTC2943_FULLSCALE_VOLTAGE = 23.6;
const float LTC2943_FULLSCALE_CURRENT = 60E-3;
const float LTC2943_FULLSCALE_TEMPERATURE = 510;

//
void batteryInit();

void batteryDeinit();

void batteryRead();

//! Calculate the LTC2943 charge in mAh
//! @return Returns the Coulombs of charge in the ACR register.
float LTC2943_code_to_mAh(uint16_t adc_code,            //!< The RAW ADC value
                          float resistor,       //!< The sense resistor value
                          uint16_t prescalar    //!< The prescalar value
                         );
