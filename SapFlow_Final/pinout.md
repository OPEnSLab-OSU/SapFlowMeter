# Pinout of Feather M0 LoRa


**Left**

|Pin  |Signal|Peripheral|
|-----|------|----------|
|RST  | Reset|
|3V   | 3.3V | Voltage regulator |
|ARef | ARef | ADC |
|GND  |
|A0   |
|A1   |
|A2   |
|A3   |
|A4   |CS   |MAX RTD sensor #2|
|A5   |CS   |MAX RTD sensor #1|
|24   |SCK  |SPI|
|23   |MOSI |SPI|
|22   |MISO |SPI|
|0    |RX   |Serial|
|1    |TX   |Serial|
|DIO1 | ??? | LoRa |

**Right**

|Pin  |Signal|Peripheral|
|-----|------|----------|
| 20 | SDA   |I2C|
| 21 | SCL   |I2C|
| 5  | 3.3_En| Power out (Pull low to enable)|
| 6  | 5v_En | Power out (Pull high to enable)|
| 9  |
| 10 | CS| SD Card on Hypno|
| 11 | Heat | Relay |
| 12 | RTC_interrupt |Real-time clock (used for alarms)|
| 13 | Status | LED |
| 5V | USB    | Power in |
| En | Enable | Voltage Regulator |
| Bat| 3.7V   | Battery |

**Pins dedicated to LoRa:**

|Pin  | Signal |
|-----|--------|
|D3   | IRQ |
|D4   | RST |
|D8   | CS  |
|DIO1 | ??? |
|DIO2 | ??? |
|DIO3 | ??? |
|DIO5 | ??? |
Note: LoRa shares the SPI peripheral, but with a seperate chip select.
