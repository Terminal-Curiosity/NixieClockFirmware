#pragma once

#include <Adafruit_NeoPixel.h>


bool ledsInit(void);
void updateLeds();

void ledRainbowFade(uint16_t updateDelayTime = 100);
void ledRainbowWave(uint16_t updateDelayTime = 50);
void ledNightRider(bool rainbow = false);
void ledBinaryCounter(uint16_t updateDelayTime = 500);

void setFourPixelsEqual(uint32_t color);

uint32_t colorHSV8(uint8_t h, uint8_t s, uint8_t v);