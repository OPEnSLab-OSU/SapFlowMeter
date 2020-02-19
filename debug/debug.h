#pragma once

#include <string.h> // for strncpy
#include "watchdog.h"

class FunctionMarker{
public:
  int line;
  const char * function;
  void print(void);
  void write(void);
  bool read(void);
private:
  char buffer[100];
};

static class FunctionMarker halt_location;

static class WatchdogSAMD wdt;

#define MARK() halt_location.line = __LINE__; \
halt_location.function = __PRETTY_FUNCTION__
//wdt.reset()
