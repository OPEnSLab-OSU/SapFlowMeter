#include "battery.h"

void batteryInit(){
  int8_t LTC2943_mode;
  int8_t ack = 0;
  //Set the control register of the LTC2943 to automatic mode as well as set prescalar and ALCC pin values.
  //Set into automatic mode, Prescaler is set too 4096, ALCC is disabled;
  LTC2943_mode = LTC2943_AUTOMATIC_MODE|LTC2943_PRESCALAR_M_4096|LTC2943_DISABLE_ALCC_PIN;

  Wire.beginTransmission(LTC2943_I2C_ADDRESS);  // begin I2C transmition
  Wire.write(LTC2943_CONTROL_REG);              // move register pointer to Control Register
  Wire.write(LTC2943_mode);                     // writes user set mode into Control Register
  Wire.endTransmission();

  Wire.beginTransmission(LTC2943_I2C_ADDRESS);  // begin I2C transmition
  Wire.write(LTC2943_ACCUM_CHARGE_MSB_REG);     // move register pointer to MSB Accumulated Charge Register
  Wire.write(0x00);                             // Writes 0 too Accumulated Charge Register MSB
  Wire.write(0x00);                             // writes 0 too Accumulated Charge Register LSB
  Wire.endTransmission();
}

void batteryDeinit(){

}
void batteryRead(int threshold){
  float Mah;
  Wire.beginTransmission(LTC2943_I2C_ADDRESS);  //Begins I2C communication
  Wire.write(LTC2943_ACCUM_CHARGE_MSB_REG);     //Points too LTC2943_ACCUM_CHARGE_MSB_REG
  Wire.endTransmission();                       //Ends transmission

  Wire.requestFrom(LTC2943_I2C_ADDRESS,2);      //Reads two bytes from the LTC2943, MSB -> LSB ACR
  uint16_t ACR = Wire.read();                   //Creates ACR, 16bit int, reads in LTC2943_ACCUM_CHARGE_MSB_REG
  ACR = ACR << 8;                               //Bit Shifts
  ACR |= Wire.read();                           //Reads in LTC2943_ACCUM_CHARGE_LSB_REG

  //Send the value of the ACR register and Find MAH, 150mOhms Resistor, 4096 Prescaler
  mAh = LTC2943_code_to_mAh(ACR, .150, LTC2943_PRESCALAR_M_4096);
  if(mAh >= threshold)

}

float LTC2943_code_to_mAh(uint16_t adc_code, float resistor, uint16_t prescalar )
// The function converts the 16-bit RAW adc_code to mAh
{
  float mAh_charge;
  mAh_charge = 1000*(float)(adc_code*LTC2943_CHARGE_lsb*prescalar*50E-3)/(resistor*4096);
  return(mAh_charge);
}
