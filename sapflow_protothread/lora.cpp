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

// Class to manage message delivery and receipt, using the driver declared above
static RHReliableDatagram manager(rf95, CLIENT_ADDRESS);

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

  while (!manager.init()) {
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
  PLOG_DEBUG << "Transmitting..."; //< Send a message to rf95_server
  // Send a message to manager_server
  if (manager.sendtoWait((uint8_t *)radiopacket, packet_len, SERVER_ADDRESS))
  {
    // Now wait for a reply from the server
    uint8_t from;   
    if (manager.recvfromAckTimeout((uint8_t *)radiopacket, &packet_len, 2000, &from))
    {
      PLOG_DEBUG.printf("got reply from %H, RSSI: %d", from, rf95.lastRssi());
      PLOG_INFO << radiopacket;
    }
    else
    {
      PLOG_WARNING << "No reply, is server running?";
    }
  }
  else
    PLOG_WARNING << "sendtoWait failed";

  digitalWrite(RFM95_CS, HIGH); //< disable LoRa
}
