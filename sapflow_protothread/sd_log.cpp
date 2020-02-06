#include "sd_log.h"

String newfile( String fname, String suffix ){
  String temp = fname + suffix;
  int i = 0;
  while(sd.exists(temp.c_str())){
    ++i;
    char ones = i%10 + '0';
    char tens = i/10 + '0';
    temp = fname + '(' + tens + ones + ')' + suffix;
  }
  return temp;
}
