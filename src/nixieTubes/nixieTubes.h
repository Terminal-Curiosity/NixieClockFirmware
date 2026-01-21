#pragma once

#include <stdint.h>

// --- Nixie Tube & PCF8574 Configuration ---
static constexpr uint8_t PCF8574_ADDRESS_1 = 0x20; // Address of your first PCF8574
static constexpr uint8_t PCF8574_ADDRESS_2 = 0x24; // Address of your second PCF8574
static constexpr uint8_t K155ID1_OFF_CODE = 0x0F; // BCD 1111 (an invalid input that turns off all cathodes on K155ID1)

void showCurrentTime();   //update the tube display to show the new time
void tubesDisplayValue(int); //update the tube display to show the value passed into the function.
    
