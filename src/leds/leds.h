#pragma once

//#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

bool ledsInit(void);
void ledSlowRainbowFade(uint16_t updateDelayTime = 100);
void setFourPixelsEqual(uint32_t color);


uint32_t colorHSV(float h, float s, float v);
// 