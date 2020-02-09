#pragma once

#include "pinout.h"

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

/** @file */

/**
Initialize the LoRa radio.

This function turns on the radio, sets the frequency,
and prepares it for use. It does not take any parameters.
*/
void lora_init(void);

/**
Builds a JSON string to send over LoRa.

Builds a JSON string containing sapflow, weight, temperature, time, and tree ID.
The string is stored in a global variable to be read by send_msg()

@param flow The calculates sapflow
@param weight The text received from the scale. Use "0" if scale is not connected.
@param temp The baseline temperature of the tree
@param time The date and time from the Real-Time Clock
*/
void build_msg(float flow, char * weight, float temp, char * time);

/**
Sends a LoRa packet to the base station.

Sends the string made by build_json() to the base station over LoRa.
Calling this function without first calling build_msg() produces undefined behavior.
*/
void send_msg( void );
