#include "measure.h"
#include "lora.h"

/// @file

float maxtemp; //< Used to store the max heater temperature

int sample_timer(struct pt *pt)
{
  PT_BEGIN(pt);
  Serial.println("Initialized timer thread");
  while(1)
  {
    sample_trigger = true;
    PT_YIELD(pt); //< Give everyone a chance to see the trigger
    sample_trigger = false; //< Clear the trigger
    PT_TIMER_DELAY(pt,1000); //< Sample every 1000ms
  }
  PT_END(pt);
}

int measure(struct pt *pt)
{
  PT_BEGIN(pt);
  Serial.print("Initializing measurement thread... ");
  static Adafruit_MAX31865 upper_rtd = Adafruit_MAX31865(UPPER_CS);
  static Adafruit_MAX31865 lower_rtd = Adafruit_MAX31865(LOWER_CS);
  static Adafruit_MAX31865 heater_rtd = Adafruit_MAX31865(HEATER_CS);
  upper_rtd.begin(MAX31865_2WIRE);
  lower_rtd.begin(MAX31865_2WIRE);
  heater_rtd.begin(MAX31865_2WIRE);
  Serial.println("Done");
  PT_YIELD(pt);
  while(1)
  {      
    // Wait for next sample trigger
    PT_WAIT_UNTIL(pt, sample_trigger );
    PT_WAIT_WHILE(pt, sample_trigger);
    // Get the latest temperature
    MARK();
    latest.upper = upper_rtd.temperature(Rnom, Rref); MARK();
    latest.lower = lower_rtd.temperature(Rnom, Rref); MARK();
    latest.heater = heater_rtd.temperature(Rnom, Rref); MARK();
    maxtemp = max(latest.upper, maxtemp); MARK();
    maxtemp = max(latest.lower, maxtemp); MARK();
    maxtemp = max(latest.heater, maxtemp); MARK();
    DateTime t = rtc_ds.now(); MARK();
    // Print to Serial terminal
    cout << "Upper: " << latest.upper << " Lower: "; MARK();
    cout << latest.lower << " Heater: " <<latest.heater; MARK();
    cout << " Time: " << t.text() << endl; MARK();
    // Save calculated sapflow
    ofstream logfile = ofstream("demo_log.csv", 
        ios::out | ios::app ); MARK();
    logfile << t.text() << ", "; MARK();
    logfile << setw(6) << latest.upper << ", "; MARK();
    logfile << setw(6) << latest.lower << ", "; MARK();
    logfile << setw(6) << latest.heater << endl; MARK();
    logfile.close();  MARK();//< Ensure the file is closed
  }
  PT_END(pt);
}

// Calculates baseline temperature
int baseline(struct pt *pt)
{
  PT_BEGIN(pt);MARK();
  Serial.print("Initializing baseline thread... ");
  // Declare persistant variable for this thread
  static int i;
  // Initialize the baseline (reference) temperature
  reference.upper = 0;
  reference.lower = 0;
  maxtemp = -300; //< Any temperature should be greater than this.
  Serial.println("Done");
  // Take an average over the first 10 seconds
  for(i = 0; i < 10; ++i){ MARK();
    PT_WAIT_UNTIL(pt, sample_trigger); MARK();
    PT_WAIT_WHILE(pt, sample_trigger); MARK();
    reference.upper += latest.upper;
    reference.lower += latest.lower;
  }; MARK();
  reference.upper /= i;
  reference.lower /= i; MARK();
  cout<<"Baseline: "<<reference.upper<<", "<<reference.lower<<endl; MARK();
  PT_END(pt);
}


// Calculates temperature delta and sapflow
int delta(struct pt *pt)
{
  PT_BEGIN(pt);MARK();
  Serial.print("Initializing delta thread... ");
  // Declare persistent variables for this thread
  static int i;
  static float flow;
  // Initialize the flow value
  flow = 0;
  Serial.println("Done");
  /** We compute sapflow using the following formula::
    sapflow = k / x * log(v1 / v2) / 3600

    - k is an empirical constant
    - x is the distance between the probes
    - v1 is temperature increase of the upper probe from its baseline temperature
    - v2 is the temperature increase of the lower probe from its baseline temperature
    - 3600 is the number of seconds in an hour

    In order to get a smoother result, we are takng the average of this calculation over a period of 40 seconds. Burges et. al. (2001) suggests that this value should converge.
  */
  for(i = 0; i < 40; ++i ){ MARK();
    PT_WAIT_UNTIL(pt, sample_trigger); MARK();
    PT_WAIT_WHILE(pt, sample_trigger); MARK();
    // Ratio of upper delta over lower delta
    float udelt = latest.upper - reference.upper;
    float ldelt = latest.lower - reference.lower;
    cout << "Delta: " << udelt <<", " << ldelt << endl; MARK();
    // Take the average before the log, since this ratio should converge
    flow += udelt / ldelt;
  }; MARK();
  cout<<"Finished measurements."<<endl; MARK();
  flow /= i; MARK();
  // Complete the rest of the equation
  flow = log(flow) * (3600.*2e-6/7e-3); MARK();
  // Print the result
  cout<<"Flow is "<<flow<<endl; MARK();
  // Write the sapflow to the file.
  ofstream sapfile = ofstream("demo.csv", ios::out | ios::app); MARK();
  char * time = rtc_ds.now().text(); MARK();
  cout << time << ", "; MARK();
  cout << reference.upper << ", "; MARK();
  cout << reference.lower << ", "; MARK();
  cout << flow << ", " << endl; MARK();
  sapfile << time << ", "<< reference.upper << ", "; MARK();
  sapfile << reference.lower << ", "<< flow << ", "<< endl; MARK();
  sapfile.close(); MARK();
  // Send the data over LoRa
  lora_init(); MARK();
  build_msg(flow, "0", reference.upper, maxtemp); MARK();
  send_msg(); MARK();
  
  PT_END(pt);
}
