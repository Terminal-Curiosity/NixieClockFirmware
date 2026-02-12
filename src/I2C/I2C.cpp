#include "I2C.h"
#include "../config.h"
#include "logger/logger.h"

bool i2cInit(void) {
    Wire.begin(SDA1_pin, SCL1_pin);  // Initialize I2C with specified pins
    Wire.setClock(100000); // Set I2C speed (default is 100kHz)
    logInfo("I2C Initialized.");
    return true;
}