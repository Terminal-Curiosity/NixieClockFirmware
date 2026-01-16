#include "I2C.h"
#include "../config.h"

void i2c_Init() {
    Wire.begin(SDA1_pin, SCL1_pin);  // Initialize I2C with specified pins
    Wire.setClock(100000); // Set I2C speed (default is 100kHz)
}