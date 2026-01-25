#pragma once

#include <Adafruit_NeoPixel.h>

bool ledsInit(void);
void updateLeds(uint16_t updateDelayTime = 100);

void ledSlowRainbowFade();
void ledSlowRainbowWave();

void setFourPixelsEqual(uint32_t color);



uint32_t colorHSV(float h, float s, float v);