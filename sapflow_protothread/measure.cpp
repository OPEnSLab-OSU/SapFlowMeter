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
    Serial.println("Trigger");
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
    Serial.print("Skipping log file...");
    static ofstream logfile = ofstream("demo_log.csv", ios::out | ios::app );
    Serial.println("Done");
    while(1)
    {      
      // Wait for next sample trigger
      PT_YIELD_UNTIL(pt, (sample_trigger || sleep));
      sample_trigger = false;
      // Check if we prepare for sleep
      if( sleep )
      {
        logfile.close();
        Serial.println("Sleeping");
        PT_YIELD(pt); // sleep occurs here
        break;
      }
      Serial.println("Sampling...");
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
  pinMode(ALARM_PIN, INPUT_PULLUP);
  DateTime t = rtc_ds.now();
  t = t + TimeSpan( 10 ); // average over 10 seconds
  rtc_ds.setAlarm(t);
  Serial.println("Done");
  while(digitalRead(ALARM_PIN))
  {
    PT_YIELD_UNTIL(pt, sample_trigger);
    reference.upper += latest.upper;
    reference.lower += latest.lower;
    ++i;
  }
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
  pinMode(ALARM_PIN, INPUT_PULLUP);
  DateTime t = rtc_ds.now();
  t = t + TimeSpan( 40 ); // Average over 40 seconds
  rtc_ds.setAlarm(t);
  Serial.println("Done");
  while(digitalRead(ALARM_PIN))
  {
    PT_YIELD_UNTIL(pt, sample_trigger);
    // Ratio of upper delta over lower delta
    flow += (latest.upper - reference.upper) / 
    (latest.lower / reference.lower);
    ++i;
  }
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
