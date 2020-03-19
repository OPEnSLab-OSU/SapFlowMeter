#include "measure.h"

/// Bitmasks for the MCP3424 status/control register
enum register_mask{
  RDY=1<<7,  ///< result ready
  CHAN=3<<5, ///< Channel 0-3
  CON=1<<4,  ///< Continuous/One-Shot
  DEPTH=3<<2,  ///< Bit depth 12-18
  PGA=3<<0,   ///< Gain of 1-8
};

/// @file

int measure(struct pt *pt, struct measure_stack &m)
{
  PT_BEGIN(pt);
  // Validate the I2C address
  m.addr = (0b1101<<3) | (m.addr & 0b111);
  // Wait for 1 second to elapse
  if( (millis() - pt->t) > 0 ){
    PT_WAIT_UNTIL(pt, (millis()-pt->t)>=1000);
  }
  pt->t = millis();

  /* read from ch1 - heater */
  PT_SPAWN(pt, &m.child, mcp3424_measure(&m.child,m.addr,1,m.raw[0]));

  /* read from ch2 - bottom */
  PT_SPAWN(pt, &m.child, mcp3424_measure(&m.child,m.addr,2,m.raw[1]));

  /* read from ch4 - top    */
  PT_SPAWN(pt, &m.child, mcp3424_measure(&m.child,m.addr,4,m.raw[2]));
  m.raw[2] = -m.raw[2]; // input wires on ch4 are backwards
  MARK;
  /* turn raw readings into temperatures */
  m.latest.heater = rtd_calc(m.raw[0]);
  m.latest.lower = rtd_calc(m.raw[1]);
  m.latest.upper = rtd_calc(m.raw[2]);
  // Set the binary semaphore
  m.sem.count = 1;
  
  //Log to the SD card
  m.maxtemp = max(m.latest.upper, m.maxtemp); MARK;
  m.maxtemp = max(m.latest.lower, m.maxtemp); MARK;
  m.maxtemp = max(m.latest.heater, m.maxtemp); MARK;
  DateTime t = rtc_ds.now(); MARK;
  // Print to Serial terminal
  cout << "Tree" << m.treeID;
  cout << " Upper: " << m.latest.upper << " Lower: "; MARK;
  cout << m.latest.lower << " Heater: " <<m.latest.heater; MARK;
  cout << " Time: " << t.text() << endl; MARK;
  // Save calculated sapflow
  String fname = "tree" + int2str(m.treeID) + "_log.csv";
  MARK;
  ofstream logfile = ofstream(fname.c_str(), 
      ios::out | ios::app ); MARK;
  logfile << t.text() << ", "; MARK;
  logfile << setw(6) << m.latest.upper << ", "; MARK;
  logfile << setw(6) << m.latest.lower << ", "; MARK;
  logfile << setw(6) << m.latest.heater << endl; MARK;
  logfile.close();  MARK;//< Ensure the file is closed
  // Loop to the beginning
  PT_RESTART(pt);
  PT_END(pt);
}

// Calculates baseline temperature
int baseline(struct pt *pt, struct measure_stack &m, char &rdv)
{
  PT_BEGIN(pt);MARK;
  // Initialize the baseline (reference) temperature
  m.reference.upper = 0;
  m.reference.lower = 0;
  m.maxtemp = -300; //< Any temperature should be greater than this.
  // Take an average over the first 10 seconds
  for(m.i = 0; m.i < 10; ++(m.i)){ MARK;
    PT_SEM_WAIT(pt, &m.sem);
    m.reference.upper += m.latest.upper;
    m.reference.lower += m.latest.lower;
  }; MARK;
  m.reference.upper /= m.i;
  m.reference.lower /= m.i; MARK;
  cout<<"Tree"<<m.treeID<<" Baseline: "<<m.reference.upper<<", "<<m.reference.lower<<endl; MARK;
  // Wait until all parties have reached the rendezvous
  --rdv;
  PT_WAIT_WHILE(pt, rdv);
  PT_END(pt);
}

// Calculates temperature delta and sapflow
int delta(struct pt *pt, struct measure_stack &m, char &rdv)
{
  PT_BEGIN(pt);MARK;
  // Initialize the flow value
  m.flow = 0;
  /** We compute sapflow using the following formula::
    sapflow = k / x * log(v1 / v2) / 3600

    - k is an empirical constant
    - x is the distance between the probes
    - v1 is temperature increase of the upper probe from its baseline temperature
    - v2 is the temperature increase of the lower probe from its baseline temperature
    - 3600 is the number of seconds in an hour

    In order to get a smoother result, we are takng the average of this calculation over a period of 40 seconds. Burges et. al. (2001) suggests that this value should converge.
  */
  for(m.i = 0; m.i < 40; ++(m.i) ){ 
    PT_SEM_WAIT(pt, &m.sem);MARK;
    // Ratio of upper delta over lower delta
    double udelt = m.latest.upper - m.reference.upper;
    double ldelt = m.latest.lower - m.reference.lower;
    cout<<"Tree"<<m.treeID<<" Delta: "<<udelt<<", "<<ldelt<<endl;
    MARK;
    // Take the average before the log, since this ratio should converge
    m.flow += udelt / ldelt;
  }; MARK;
  cout<<"Tree"<<m.treeID<<" Finished measurements."<<endl; MARK;
  m.flow /= m.i; MARK;
  // Complete the rest of the equation
  m.flow = log(m.flow) * (3600.*2e-6/7e-3); MARK;
  // Print the result
  cout<<"Flow is "<<m.flow<<endl; MARK;
  // Write the sapflow to the file.
  String fname = "tree" + int2str(m.treeID) + "_sapflow.csv";
  MARK;
  ofstream sapfile = ofstream(fname.c_str(), 
                              ios::out | ios::app); MARK;
  char * time = rtc_ds.now().text(); MARK;
  cout << time << ", "; MARK;
  cout << m.reference.upper << ", "; MARK;
  cout << m.reference.lower << ", "; MARK;
  cout << m.flow << ", " << endl; MARK;
  sapfile << time << ", "<< m.reference.upper << ", "; MARK;
  sapfile << m.reference.lower << ", "<< m.flow << ", "<< endl; MARK;
  sapfile.close(); MARK;
  // Lora not set up
  
  // Wait until all parties have reached the rendezvous
  --rdv;
  PT_WAIT_WHILE(pt, rdv);
  PT_END(pt);
}

double rtd_calc(int32_t raw){
  double volts = -raw * (2.048 / (1UL<<20)); // Vref is 2.048, 17 bits, PGA is 8
  double ratio = volts * (7.04 / 3.3); // 3.3V supply, voltage divider using 604-ohm resistor (and 100-ohm RTD)
  double celcius = ratio * (1/3850e-6); // 3850ppm/K is standard for platinum RTD
  return(celcius);
}

int mcp3424_measure(struct pt * pt, uint8_t addr, uint8_t ch, int32_t &result){
    PT_BEGIN(pt);
    uint8_t cfg = RDY | DEPTH | PGA;  //18-bit depth, 8x PGA gain
    int32_t data = 0;
    --ch; // Change from 1-4 to 0-3
    ch = CHAN & (ch<<5);
    cfg |= ch;
    // Start the conversion
    MARK;
    Wire.beginTransmission(addr);
    Wire.write(cfg);
    Wire.endTransmission();
    MARK;

    // Wait for the result
    // 1/3.75Hz = 267ms
    PT_TIMER_DELAY(pt,260);
    do{
      PT_TIMER_DELAY(pt,5);
      MARK;
      Wire.requestFrom(addr,4);
      data = Wire.read(); // top two bits
      data <<= 8;
      data |= Wire.read(); // middle byte
      data <<= 8;
      data |= Wire.read(); // lower byte
      cfg = Wire.read();
      MARK;
    }while(cfg & RDY);

    // Extend the sign
    if( data & 0x20000 ){
      data |= 0xFFFC0000;
    }
    result = data;
    PT_END(pt);
}
