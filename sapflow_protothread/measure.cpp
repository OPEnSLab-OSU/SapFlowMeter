#include "measure.h"
#include "lora.h"

float maxtemp; ///< Used to store the max heater temperature

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
    PT_WAIT_UNTIL(pt, (sample_trigger || sleep));
    // Check if we prepare for sleep
    if( sleep )
    {
      // This seems to never be called.
      Serial.println("Sleeping");
      PT_WAIT_WHILE(pt, sleep); // sleep occurs here
      break;
    }
    PT_WAIT_WHILE(pt, sample_trigger);
    //Serial.println("Sampling...");
    // Get the latest temperature
    MARK();
    latest.upper = upper_rtd.temperature(Rnom, Rref); MARK();
    latest.lower = lower_rtd.temperature(Rnom, Rref); MARK();
    latest.heater = heater_rtd.temperature(Rnom, Rref); MARK();
    DateTime t = rtc_ds.now(); MARK();
    // Print to Serial terminal
    cout << "Upper: " << latest.upper << " Lower: "; MARK();
    cout << latest.lower << " Heater: " <<latest.heater; MARK();
    cout << " Time: " << t.text() << endl; MARK();
    // Save calculated sapflow
    cout<<"Logfile..."; MARK();
    ofstream logfile = ofstream("demo_log.csv", 
        ios::out | ios::app ); MARK();
    cout<<"opened..."; MARK();
    logfile << t.text() << ", "; MARK();
    logfile << setw(6) << latest.upper << ", "; MARK();
    logfile << setw(6) << latest.lower << ", "; MARK();
    logfile << setw(6) << latest.heater << endl; MARK();
    logfile.close();  MARK();// Ensure the file is closed
    cout<<"done"<<endl; MARK();
  }
  PT_END(pt);
}

// Calculates baseline temperature
int baseline(struct pt *pt)
{
  PT_BEGIN(pt);
  Serial.print("Initializing baseline thread... ");
  // Declare persistant variable for this thread
  static int i;
  // Initialize the baseline (reference) temperature
  reference.upper = 0;
  reference.lower = 0;
  Serial.println("Done");
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
  PT_BEGIN(pt);
  Serial.print("Initializing delta thread... ");
  // Declare persistent variables for this thread
  static int i;
  static float flow;
  maxtemp = latest.heater; // This should be the heater peak temperature
  // Initialize the flow value
  flow = 0;
  Serial.println("Done");
  for(i = 0; i < 5; ++i ){ MARK();
    PT_WAIT_UNTIL(pt, sample_trigger); MARK();
    PT_WAIT_WHILE(pt, sample_trigger); MARK();
    // Ratio of upper delta over lower delta
    float udelt = latest.upper - reference.upper;
    float ldelt = latest.lower - reference.lower;
    cout << "Delta: " << udelt <<", " << ldelt << endl; MARK();
    flow += udelt / ldelt;
  }; MARK();
  cout<<"Finished measurements."<<endl; MARK();
  flow /= i; MARK();
  flow = log(flow) * (3600.*2e-6/7e-3); MARK();
  cout<<"Flow is "<<flow<<endl; MARK();
  // Write the sapflow to the file.
  cout<<"Opening logfile..."; MARK();
  ofstream sapfile = ofstream("demo.csv", ios::out | ios::app); MARK();
  cout<<"Done"<<endl; MARK();
  char * time = rtc_ds.now().text(); MARK();
  cout << time << ", "; MARK();
  cout << reference.upper << ", "; MARK();
  cout << reference.lower << ", "; MARK();
  cout << flow << ", " << endl; MARK();
  sapfile << time << ", "<< reference.upper << ", "; MARK();
  sapfile << reference.lower << ", "<< flow << ", "<< endl; MARK();
  sapfile.close(); MARK();
  lora_init(); MARK();
  build_msg(flow, "0", reference.upper, maxtemp); MARK();
  send_msg(); MARK();
  
  PT_END(pt);
}
