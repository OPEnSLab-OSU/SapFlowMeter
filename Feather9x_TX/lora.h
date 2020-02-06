#pragma once

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

enum lora_pins {
  RFM95_CS = 8,
  RFM95_RST = 4,
  RFM95_INT = 3,
};

void lora_init(void);

void build_msg(float flow, float weight, float temp, char * time);

void send_msg( void );
