// Feather9x_RX
// Designed to work with Feather9x_TX

#include <RHReliableDatagram.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <Ethernet.h>
#include <ArduinoJson.h>
#include <FeatherFault.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

/* for Feather32u4 RFM9x
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
*/

/* for feather m0 RFM9x */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

/* for shield 
#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 7
*/

/* Feather 32u4 w/wing
#define RFM95_RST     11   // "A"
#define RFM95_CS      10   // "B"
#define RFM95_INT     2    // "SDA" (only SDA/SCL/RX/TX have IRQ!)
*/

/* Feather m0 w/wing 
#define RFM95_RST     11   // "A"
#define RFM95_CS      10   // "B"
#define RFM95_INT     6    // "D"
*/

#if defined(ESP8266)
  /* for ESP w/featherwing */ 
  #define RFM95_CS  2    // "E"
  #define RFM95_RST 16   // "D"
  #define RFM95_INT 15   // "B"

#elif defined(ESP32)  
  /* ESP32 feather w/wing */
  #define RFM95_RST     27   // "A"
  #define RFM95_CS      33   // "B"
  #define RFM95_INT     12   //  next to A

#elif defined(NRF52)  
  /* nRF52832 feather w/wing */
  #define RFM95_RST     7   // "A"
  #define RFM95_CS      11   // "B"
  #define RFM95_INT     31   // "C"
  
#elif defined(TEENSYDUINO)
  /* Teensy 3.x w/wing */
  #define RFM95_RST     9   // "A"
  #define RFM95_CS      10   // "B"
  #define RFM95_INT     4    // "C"
#endif


// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(rf95, SERVER_ADDRESS);

// Blinky on receipt
#define LED 13

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
//byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}
//This address is registered on Resnet (dorm)
//byte mac[] = {0x1A, 0x41, 0x3E, 0x1F, 0x18, 0xF5};
//This address is registered on OSU access
byte mac[] = {0xC6, 0x1b, 0xa6, 0xac, 0xd7, 0x03};

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
//char server[] = "wifitest.adafruit.com";    // name address for Google (using DNS)
char server[] = "web.engr.oregonstate.edu";    // name address for Google (using DNS)

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(128, 193, 200, 177);
//IPAddress myDns(192, 168, 0, 1);
IPAddress myDns(128, 193, 15, 12);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

// Variables to measure the speed
unsigned long beginMicros, endMicros;
unsigned long byteCount = 0;
bool printWebData = true;  // set to false for better speed measurement

void setup()
{
  FeatherFault::StartWDT(FeatherFault::WDTTimeout::WDT_4S);
  MARK;
  pinMode(LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  MARK;
  Serial.begin(115200);
  delay(100);

  Serial.println("Feather LoRa RX Test!");
  MARK;

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  MARK;

  while (!manager.init()) {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    MARK;
    while (1);
  }
  MARK;
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    MARK;
    while (1);
  }
  MARK;
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
  MARK;
}

uint8_t data[] = "And hello back to you";
// Dont put this on the stack:
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

void loop()
{
  MARK;
  if (manager.available())
  {
    MARK;
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from))
    {
      MARK;
      digitalWrite(LED, HIGH);
      Serial.print("got request from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
      MARK;

      // Send a reply back to the originator client
      if (!manager.sendtoWait(data, sizeof(data), from))
        Serial.println("sendtoWait failed"); MARK;
      digitalWrite(LED, LOW);
      MARK;
    }
  MARK;
     // You can use Ethernet.init(pin) to configure the CS pin
  Ethernet.init(10);  // Most Arduino shields [including Feather M0]
  pinMode(RFM95_RST, INPUT_PULLUP);
  MARK;

  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    MARK;
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      MARK;
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    MARK;
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    MARK;
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  MARK;
  } else {
    MARK;
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  MARK;
    delay(1000);
  MARK;
  Serial.print("connecting to ");
  Serial.print(server);
  Serial.println("...");
  MARK;
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    MARK;
    Serial.print("connected to ");
    Serial.println(client.remoteIP());
    MARK;
    // Make a HTTP request:
    char input[] = "{\"flow\":\"11.3\",\"weight\":\"7.8\",\"temp\":\"27.5\",\"time\":\"15:30\",\"id\":\"15\"}";
    MARK;
    const int capacity = JSON_OBJECT_SIZE(5);
    MARK;
    StaticJsonDocument<capacity> doc;
    MARK;
    DeserializationError err = deserializeJson(doc, (char*)buf);
    MARK;
    if (err) {
      MARK;
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(err.c_str());
    }
    MARK;

    const char * flow = doc["flow"];
    MARK;
    const char * weight = doc["weight"];
    MARK;
    const char * temp = doc["temp"];
    MARK;
    const char * time1 = doc["time"];
    MARK;
    const char * id = doc["id"];
    MARK;
  
  char url[100];
    MARK;
   sprintf(url, "GET /~veselyv/new.php?flow=%s&weight=%s&temp=%s&time=%s&id=%s HTTP/1.1",flow,weight,temp,time1,id);
    MARK;
   client.println(url);
   MARK;
   Serial.print("Sent url: ");
   Serial.println(url);
   MARK;
   client.println("Host: web.engr.oregonstate.edu");
   client.println("Connection: close");
   client.println();
   MARK;
   
  } else {
    MARK;
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  MARK;
  delay(1000);
  MARK;
  client.stop();
  MARK;
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  }
  MARK;
}
