#pragma once

#include <Adafruit_NeoPixel.h>

constexpr uint8_t LED_NUM_PIXELS = 4;

bool ledsInit(void);
void updateLeds();
void ledsShow();
void ledsSetPixel(uint8_t i, uint8_t r, uint8_t g, uint8_t b);
void ledsSetPixelPacked(uint8_t i, uint32_t RGB);
void setFourPixelsEqual(uint32_t color);

uint32_t ledsColour(uint8_t r, uint8_t g, uint8_t b);
uint32_t colorHSV8(uint8_t h, uint8_t s, uint8_t v);