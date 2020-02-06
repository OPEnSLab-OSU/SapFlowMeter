#include "measure.h"

static bool sample_trigger;

#define Rnom 100.0
#define Rref 430.0

int sample_timer(struct pt *pt)
{
  PT_BEGIN(pt);
  Serial.println("Initialized timer thread");
  while(1)
  {
    sample_trigger = true;
    // Serial.println("Trigger");
    PT_YIELD(pt); // Give everyone a chance to see the trigger
    sample_trigger = false; // Clear the trigger
    PT_TIMER_DELAY(pt,1000); // Sample every 1000ms
  }
  PT_END(pt);
}

int measure(struct pt *pt)
{
  PT_BEGIN(pt);
  Serial.print("Initializing measurement thread... ");
  static Adafruit_MAX31865 upper_rtd = Adafruit_MAX31865(A5);
  static Adafruit_MAX31865 lower_rtd = Adafruit_MAX31865(A4);
  static Adafruit_MAX31865 heater_rtd = Adafruit_MAX31865(A3);
  upper_rtd.begin(MAX31865_2WIRE);
  lower_rtd.begin(MAX31865_2WIRE);
  heater_rtd.begin(MAX31865_2WIRE);
  Serial.println("Done");
  PT_YIELD(pt);
  while(1)
  {
    Serial.print("Opening log file...");
    static ofstream logfile = ofstream("demo_log.csv", 
                                       ios::out | ios::app );
    Serial.println("Done");
    while(1)
    {      
      // Wait for next sample trigger
      PT_YIELD_UNTIL(pt, (sample_trigger || sleep));
      // Check if we prepare for sleep
      if( sleep )
      {
        logfile.close();
        Serial.println("Sleeping");
        PT_YIELD(pt); // sleep occurs here
        break;
      }
      PT_YIELD(pt);
      //Serial.println("Sampling...");
      // Get the latest temperature
      latest.upper = upper_rtd.temperature(Rnom, Rref);
      latest.lower = lower_rtd.temperature(Rnom, Rref);
      latest.heater = heater_rtd.temperature(Rnom, Rref);
      // Save calculated sapflow
      DateTime t = rtc_ds.now();
      Serial.print("Upper: ");
      Serial.print(latest.upper);
      Serial.print(" Lower: ");
      Serial.print(latest.lower);
      Serial.print(" Heater: ");
      Serial.print(latest.heater);
      Serial.print(" Time: ");
      Serial.println(t.text());
      logfile << t.text() << ", ";
      logfile << setw(6) << latest.upper << ", ";
      logfile << setw(6) << latest.lower << ", ";
      logfile << setw(6) << latest.heater << endl;
    }
  }
  PT_END(pt);
}

// Calculates baseline temperature
int baseline(struct pt *pt)
{
  PT_BEGIN(pt);
  Serial.print("Initializing baseline thread... ");
  static int i = 0;
  reference = latest;
  (pt)->t = millis() + 10000; // loop for 10 seconds
  Serial.println("Done");
  do{
    PT_WAIT_UNTIL(pt, sample_trigger);
    PT_WAIT_WHILE(pt, sample_trigger);
    Serial.println("Baseline thd");
    reference.upper += latest.upper;
    reference.lower += latest.lower;
    ++i;
  }while(millis()<(pt)->t);
  reference.upper /= i;
  reference.lower /= i;
  PT_END(pt);
}


// Calculates temperature delta and sapflow
int delta(struct pt *pt)
{
  PT_BEGIN(pt);
  Serial.print("Initializing delta thread... ");
  static int i = 0;
  static float flow = 0;
  (pt)->t = millis() + 40000; // loop for 40 seconds
  Serial.println("Done");
  do{
    PT_WAIT_UNTIL(pt, sample_trigger);
    PT_WAIT_WHILE(pt, sample_trigger);
    Serial.println("Delta thd");
    // Ratio of upper delta over lower delta
    flow += (latest.upper - reference.upper) / 
    (latest.lower - reference.lower);
    ++i;
  }while(millis()<(pt)->t);
  flow /= i;
  flow = log(flow) * 3600.;
  // Write the sapflow to the file.
  ofstream sapfile = ofstream("demo.csv", ios::out | ios::app);
  sapfile << rtc_ds.now().text() << ", ";
  sapfile << reference.upper << ", ";
  sapfile << reference.lower << ", ";
  sapfile << flow << ", ";
  sapfile << read_weight() << endl;
  sapfile.close();
  PT_END(pt);
}
