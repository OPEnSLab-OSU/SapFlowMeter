#pragma once

#include <string.h> // for strncpy

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

#define MARK() halt_location.line = __LINE__; \
halt_location.function = __PRETTY_FUNCTION__
