#include "sd_log.h"

/// @file

String newfile( String fname, String suffix ){
  String temp = fname + suffix; //< Start with the name provided
  int i = 0;
  // Try appending a number
  while(sd.exists(temp.c_str()) && (i<100)){
    // If the first number didn't work, keep trying
    ++i;
    char ones = i%10 + '0';
    char tens = i/10 + '0';
    temp = fname + '(' + tens + ones + ')' + suffix;
  }
  return temp;
}

String int2str( int32_t x ){
  char buf[12];
  dtoa(x, buf);
  String s = buf;
  return(s);
}
