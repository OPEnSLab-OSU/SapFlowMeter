#pragma once

#include "pinout.h"

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

void lora_init(void);

void build_msg(float flow, char * weight, float temp, char * time);

void send_msg( void );
