#pragma once

#include "leds/leds.h"

void ledBlank();
void ledRainbowFade(uint16_t updateDelayTime = 100);
void ledRainbowWave(uint16_t updateDelayTime = 50);
void ledNightRider(bool rainbow = false);
void ledBinaryCounter();
void ledPulseShockwave();
void ledHeartbeat();