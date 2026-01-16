#pragma once

//#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

void ledsInit();
void ledSlowRainbowFade();
void setFourPixelsEqual(uint32_t color);


uint32_t colorHSV(float h, float s, float v);
// 