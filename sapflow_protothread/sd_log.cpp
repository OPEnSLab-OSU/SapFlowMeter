#include "sd_log.h"

/// @file

String newfile( String fname, String suffix ){
  String temp = fname + suffix; //< Start with the name provided
  int i = 0;
  // Try appending a number
  while(sd.exists(temp.c_str())){
    // If the first number didn't work, keep trying
    // Note that this will hang if 00 through 99 are all taken
    ++i;
    char ones = i%10 + '0';
    char tens = i/10 + '0';
    temp = fname + '(' + tens + ones + ')' + suffix;
  }
  return temp;
}
