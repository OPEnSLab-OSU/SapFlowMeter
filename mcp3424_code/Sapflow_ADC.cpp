#include "Sapflow_ADC.h"

/***************************** for reference
 * struct temperature{
 *     double upper;   //temperature in deg C at upper probe
 *     double lower;   //temperature in deg C at lower probe
 *     double heater;  //temperature in deg C at heater probe
 * }
 *****************************/
int get_temp(struct pt *pt, uint8_t i2c_addr, struct temperature *dest){
    
    Wire.begin();
    int32_t result[3];  //stores raw readout from ADC
    double deg_c[3];    //stores interpolated temperature
    uint8_t addr = (0b1101<<3) | (i2c_addr & 0b111);

    /* read from ch1 - heater */
    result[0] = measure(addr, 1);

    /* read from ch2 - bottom */
    result[1] = measure(addr, 2);

    /* read from ch4 - top    */
    result[2] = -measure(addr, 4); // input wires on ch4 are backwards

    /* turn raw readings into temperatures */
    uint8_t i;
    for(i=0;i<2;i++){
        deg_c[i] = rtd_calc(result[i]);
    }
    
    /* update the temperature struct */
    dest->heater = deg_c[0];
    dest->lower  = deg_c[1];
    dest->upper  = deg_c[2];

    return 0;
}

double rtd_calc(int32_t raw){
  double volts = -raw * (2.048 / (1UL<<20)); // Vref is 2.048, 17 bits, PGA is 8
  double ratio = volts * (7.04 / 5.0); // 5V supply, voltage divider using 604-ohm resistor (and 100-ohm RTD)
  double celcius = ratio * (1/3850e-6); // 3850ppm/K is standard for platinum RTD
  return(celcius);
}

int32_t measure(uint8_t addr, uint8_t ch){
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
    do{
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
    return( data );
}
