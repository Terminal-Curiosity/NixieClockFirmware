#pragma once
#include <stdint.h>

// Firefly synchronization animation
// Pulse-coupled oscillators that gradually sync their flashes.
//
// basePeriodMs:
//   Approximate natural flash period for each LED.
//   Each firefly gets a small random offset from this.
void ledFireflySync(uint16_t basePeriodMs = 1400);
