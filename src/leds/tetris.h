#pragma once

#include "leds.h"
struct LedTetrisState {
  uint16_t score;
  bool gameOver;
};


LedTetrisState ledTetrisUpdate(uint16_t stepMs = 1000);
void ledTetrisReset();