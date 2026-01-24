#pragma once

#include <stdint.h>

enum DisplayMode {
  MODE_NORMAL_TIME,
  MODE_CONDITIONING,
  MODE_SET_TIME,
  MODE_ALARM_RINGING,
  MODE_ALARM_SET,
  MODE_UNKNOWN
};

// --- Nixie Tube & PCF8574 Configuration ---
static constexpr uint8_t PCF8574_ADDRESS_1 = 0x20; // Address of your first PCF8574
static constexpr uint8_t PCF8574_ADDRESS_2 = 0x24; // Address of your second PCF8574
static constexpr uint8_t K155ID1_OFF_CODE = 0x0F; // BCD 1111 (an invalid input that turns off all cathodes on K155ID1)

void nixieTubesUpdate();
void renderNormalTime();   //update the tube display to show the new time
void tubesDisplayValue(uint16_t value_to_display = 6666); //update the tube display to show the value passed into the function.
void renderConditioning();
