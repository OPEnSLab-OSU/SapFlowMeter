#include "lora.h"
#include "sd_log.h"

/// @file

#include <RHReliableDatagram.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <ArduinoJson.h>
#include <ftoa.h> //< for float to string conversion

//< Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void lora_init(void) 
{
  digitalWrite(RFM95_CS, LOW); //< enable LoRa
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    PLOG_ERROR << "LoRa radio init failed";
    PLOG_DEBUG << "Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info";
    while (1);
  }
  PLOG_INFO << "LoRa radio init OK!";

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    PLOG_ERROR << "setFrequency failed";
    while (1);
  }
  PLOG_INFO << "Set Freq to: "<< RF95_FREQ;
  
  /** Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

 The default transmitter power is 13dBm, using PA_BOOST.
 If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
 you can set transmitter powers from 5 to 23 dBm: */
  rf95.setTxPower(23, false);
  digitalWrite(RFM95_CS, HIGH); //< disable LoRa
}

static int16_t packetnum = 0;  //< packet counter, we increment per xmission

static char radiopacket[RH_RF95_MAX_MESSAGE_LEN];
static uint8_t packet_len; //< Max message length is 251, so we won't overflow

void build_msg(float flow, float temp, int treeID )
{
  PLOG_DEBUG << "Building message...";
  const int capacity=JSON_OBJECT_SIZE(10);
  StaticJsonDocument<capacity>doc;
  char str1[15];
  char str2[15];
  char str3[15];
  char * tstring = rtc_ds.now().text();
  tstring += 11; //< skip the date, just use the time
  ftoa(flow, str1);
  ftoa(temp, str3);
  doc["flow"].set(str1);
  doc["temp"].set(str3);
  doc["id"].set(treeID);
  doc["time"].set(tstring);
  packet_len = serializeJson(doc,radiopacket);
  radiopacket[packet_len] = 0; // null-terminate
  PLOG_DEBUG << radiopacket;
}


void send_msg( void )
{
  digitalWrite(RFM95_CS, LOW); //< enable LoRa
  // Send a message to manager_server
  rf95.send((uint8_t *)radiopacket, packet_len);
  PLOG_DEBUG << "Transmitting..."; //< Send a message to rf95_server
  rf95.waitPacketSent();
  PLOG_DEBUG << "Packet sent.";
    // Now wait for a reply from the server
  if (rf95.waitAvailableTimeout(1000))
  { 
    // Should be a reply message for us now   
    if (rf95.recv((uint8_t*)radiopacket, &packet_len))
    {
      PLOG_INFO << "Got reply: "<< radiopacket;
      PLOG_INFO << "RSSI: " << rf95.lastRssi();
    }
    else
    {
      PLOG_WARNING << "Receive failed";
    }
  }
  else
  {
    PLOG_WARNING << "No reply, is there a listener around?";
  }

  digitalWrite(RFM95_CS, HIGH); //< disable LoRa
}
