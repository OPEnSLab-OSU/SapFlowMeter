#define PT_USE_TIMER
#define PT_USE_SEM

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
  UPPER_CS = A5,
  LOWER_CS = A4,
  HEATER_CS = A3,
};

#include <pt.h>
#include "schedule.h"
static struct pt_sem sem_LED;
unsigned char i;

void setup() {
  //Set Hardware
  pinMode(13,OUTPUT);
  Serial.begin(115200);
  //Initialize Semaphore
  PT_SEM_INIT(&sem_LED,1);
  //Initialize the Threads
  PT_INIT(&sched_thd);
}

void loop() {
  //Check each thread by priority
  schedule(&sched_thd);
}
