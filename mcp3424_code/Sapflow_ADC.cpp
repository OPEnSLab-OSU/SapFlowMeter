#include "Sapflow_ADC.h"

int get_temp(struct get_temp_t &s){
    PT_BEGIN(&s.pt);
    // Validate the I2C address
    s.addr = (0b1101<<3) | (s.addr & 0b111);
    // Wait for 1 second to elapse
    if( (millis() - s.pt.t) > 0 ){
      PT_WAIT_UNTIL(&s.pt, (millis()-s.pt.t)>=1000);
    }
    s.pt.t = millis();

    /* read from ch1 - heater */
    PT_SPAWN(&s.pt, &s.child, mcp3424_measure(&s.child,s.addr,1,s.raw[0]));

    /* read from ch2 - bottom */
    PT_SPAWN(&s.pt, &s.child, mcp3424_measure(&s.child,s.addr,2,s.raw[1]));

    /* read from ch4 - top    */
    PT_SPAWN(&s.pt, &s.child, mcp3424_measure(&s.child,s.addr,4,s.raw[2]));
    s.raw[2] = -s.raw[2]; // input wires on ch4 are backwards

    /* turn raw readings into temperatures */
    uint8_t i;
    for(i=0;i<3;i++){
        s.deg_c[i] = rtd_calc(s.raw[i]);
    }
     
    // Set the binary semaphore
    s.sem.count = 1;
    
    //FIXME: Log to the SD card
    
    // Loop to the beginning
    PT_RESTART(&s.pt);

    PT_END(&s.pt);
}

double rtd_calc(int32_t raw){
  double volts = -raw * (2.048 / (1UL<<20)); // Vref is 2.048, 17 bits, PGA is 8
  double ratio = volts * (7.04 / 5.0); // 5V supply, voltage divider using 604-ohm resistor (and 100-ohm RTD)
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
    Wire.beginTransmission(addr);
    Wire.write(cfg);
    Wire.endTransmission();

    // Wait for the result
    // 1/3.75Hz = 267ms
    PT_TIMER_DELAY(pt,260);
    do{
      PT_TIMER_DELAY(pt,5);
      Wire.requestFrom(addr,4);
      data = Wire.read(); // top two bits
      data <<= 8;
      data |= Wire.read(); // middle byte
      data <<= 8;
      data |= Wire.read(); // lower byte
      cfg = Wire.read();
    }while(cfg & RDY);

    // Extend the sign
    if( data & 0x20000 ){
      data |= 0xFFFC0000;
    }
    result = data;
    PT_END(pt);
}
