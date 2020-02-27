#include "weight.h"

/// @file

// Gets the weight from the scale
char * read_weight( char * buffer, int len ){
  // Clear the buffer in case someone was messing with the cable
  int n = Serial1.available();
  while( Serial1.available() > len ){
    Serial1.readBytes(buffer, len);
  }
  if( Serial1.available() ){
    Serial1.readBytes(buffer, Serial1.available());
  }
  
  // Now request the weight
  Serial1.print("P\r\n");
  len = 1 + Serial1.readBytesUntil('\n', buffer, len-1);
  buffer[len] = 0;  // Null-terminate the string
  int start_index, value_length, i;
  // Filter out spaces and linefeed
  for( i = 0; buffer[i]; ++i){
    if( buffer[i] >= '0' )
      break;
  }
  start_index = i;
  for( ; buffer[i]; ++i){
    if( buffer[i] < '.' )
      break;
  }
  value_length = i - start_index;

  memmove(buffer, buffer+start_index, value_length);
  buffer[value_length] = 0; // null terminate
  return( buffer );
}
