#include <Wire.h>

// --- I2C Write Helper Function ---
void pcf8574WriteOneByte(uint8_t address, uint8_t data) {
  Wire.beginTransmission(address); 
  Wire.write(data);                 
  Wire.endTransmission();           
}
