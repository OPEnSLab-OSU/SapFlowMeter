#pragma once

#include "pinout.h"
#include "schedule.h"
#include "sd_log.h"
#include "weight.h"
#include <Adafruit_MAX31865.h>

struct temperature{
  float upper;
  float lower;
  float heater;
};

static struct temperature latest;
static struct temperature reference;

static struct pt measure_thd, sample_timer_thd, baseline_thd, delta_thd;

// Captures measurements
int measure(struct pt *pt = &measure_thd);

// Controls timing of the measurements
int sample_timer(struct pt *pt = &sample_timer_thd);

// Calculates baseline temperature
int baseline(struct pt *pt = &baseline_thd);

// Calculates temperature delta and sapflow
int delta(struct pt *pt = &delta_thd);
