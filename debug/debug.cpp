#include "debug.h"
#include <FlashStorage.h>

FlashStorage(persistent, class FunctionMarker);

void FunctionMarker::print(void){
  Serial.print("Halted at ");
  Serial.print(function);
  Serial.print(", line ");
  Serial.println(line);
}

void FunctionMarker::write(void){
  // Copy pointed value to buffer
  strncpy(buffer, function, 100);
  function = buffer;
  // Then save to the flash
  persistent.write(*this);
}

bool FunctionMarker::read(void){
  // Read the value from memory
  *this = persistent.read();

  // If we stored this before, then the function points to the buffer.
  return (function==buffer);
}
