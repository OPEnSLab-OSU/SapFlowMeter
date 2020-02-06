#pragma once

#include "pinout.h"
#include <string.h> // for memmove

static char weight_buf[20];

// Gets the weight from the scale
char * read_weight( char * buffer = weight_buf, int len=20 );
