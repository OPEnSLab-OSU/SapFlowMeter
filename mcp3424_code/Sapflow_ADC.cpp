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
    uint8_t cfgbyte = 0b00001111; //18 bit, 8x pga, one-shot, ch1
    Wire.beginTransmission(addr);
    Wire.write(cfgbyte);
    Wire.endTransmission();
    newConversion();
    result[0] = measure();

    /* read from ch2 - bottom */
    uint8_t cfgbyte = 0b00101111; //18 bit, 8x pga, one-shot, ch2
    Wire.beginTransmission(addr);
    Wire.write(cfgbyte);
    Wire.endTransmission();
    newConversion();
    result[1] = measure();

    /* read from ch4 - top    */
    uint8_t cfgbyte = 0b01101111; //18 bit, 8x pga, one-shot, ch4
    Wire.beginTransmission(addr);
    Wire.write(cfgbyte);
    Wire.endTransmission();
    newConversion();
    result[2] = measure();

    /* turn raw readings into temperatures */
    uint8_t i;
    for(i=0;i<2;i++){
        deg_c[i] = (415.25 * result[i] + 151000000.0) 
            / (0.246125 * result[i] - 540580.0) - 279.3296;
    }
    //different calculation for ch4 because I hooked up the input wires backwards
    deg_c[2] = (415.25 * result[2] - 151000000.0) 
        / (0.246125 * result[2] + 540580.0) - 279.3296;
    
    /* update the temperature struct */
    dest->heater = deg_c[0];
    dest->lower  = deg_c[1];
    dest->upper  = deg_c[2];

    return 0;
}
uint8_t getConfiguration(){
    Wire.requestFrom(_adresse, _resolution==RESOLUTION_18_BITS?4:3);
    uint8_t i=0;
    while(Wire.available()) _buffer[i++] = Wire.read();
    return (_buffer[(_resolution==RESOLUTION_18_BITS?3:2)]);
}

void newConversion(){
    uint8_t cfgbyte=getConfiguration();
    Wire.beginTransmission(_adresse);
    Wire.write(cfgbyte|=128);
    Wire.endTransmission();
}

bool isConversionFinished(){
    uint8_t result= !(getConfiguration() & 0b10000000);
    return result;
}

void MCP342x::getRawDatas(uint8_t buffer[4]){
    for(int i=0;i<4;i++) buffer[i]=_buffer[i];
}

int32_t MCP342x::measure(){
    union resultUnion{
        uint8_t asBytes[4];
        int32_t asLong;
    } result;
    while(isConversionFinished()==0); //change to allow PT
    result.asBytes[3] = _buffer[0]&0x80?0xFF:0x00;
    result.asBytes[2] = _buffer[0];
    result.asBytes[1] = _buffer[1];
    result.asBytes[0] = _buffer[2];
    return result.asLong;
}
