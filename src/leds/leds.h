#pragma once

#include <Adafruit_NeoPixel.h>

enum LedMode : uint8_t {
  LEDMODE_RAINBOW_FADE = 0,
  LEDMODE_RAINBOW_WAVE,
  LEDMODE_NIGHTRIDER,
  LEDMODE_BINARY_COUNTER,
  LEDMODE_TETRIS_DEMO,
  LEDMODE_PULSE_SHOCKWAVE,
  LEDMODE_HEARTBEAT,
  LEDMODE_CALIFE,
  LEDMODE_FIREFLY,

  LEDMODE_ENUM_COUNT, // everything before this is cyclable

  //special modes not accessed during regular cycling
  LEDMODE_OFF = 100,
  LEDMODE_TETRIS_GAME
};


constexpr uint8_t LED_NUM_PIXELS = 4;

bool ledsInit(void);
void updateLeds();
void ledsShow();
void ledsClear();
void ledsSetPixel(uint8_t i, uint8_t r, uint8_t g, uint8_t b);
void ledsSetPixelPacked(uint8_t i, uint32_t RGB);
void setFourPixelsEqual(uint32_t color);

uint32_t ledsColour(uint8_t r, uint8_t g, uint8_t b);
uint32_t colorHSV8(uint8_t h, uint8_t s, uint8_t v);