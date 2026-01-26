#include <stdint.h>
#include "config.h"
#include "logger/logger.h"
#include "ldr/ldr.h"
#include "leds.h"

//also include a simple function to set LEDs off


static uint8_t baseHue = 0;

void ledBlank()
{
    ledsSetPixel(1,  0,  0,  0);
    ledsSetPixel(2,  0,  0,  0);
    ledsSetPixel(3,  0,  0,  0);
    ledsSetPixel(4,  0,  0,  0);
}

void ledRainbowFade(uint16_t updateDelayTime) {

  static uint32_t lastUpdateMs = 0;
  
  if (millis() - lastUpdateMs < updateDelayTime) return;
  lastUpdateMs = millis();

  static uint32_t color = colorHSV8(baseHue, 255, 255); //hue from 0-255, saturation 0-255, value 0-255
  setFourPixelsEqual(color);
  baseHue++;
}

void ledRainbowWave(uint16_t updateDelayTime) {

  static uint32_t lastUpdateMs = 0;
  static uint8_t offset = 15;
  
  if (millis() - lastUpdateMs < updateDelayTime) return;
  lastUpdateMs = millis();

  for (uint8_t i = 0; i < LED_NUM_PIXELS; i++) {
        ledsSetPixelPacked(i, colorHSV8(baseHue + i*offset, 255, 255));
    }

  ledsShow();
  baseHue++;
}

void ledNightRider(bool rainbow)
{
    // const uint8_t n = strip.numPixels();
    const uint8_t n = 4;
    if (n < 2) return;

    // 1 cycle/sec, steps = 2*(n-1) per cycle
    const uint16_t stepsPerCycle = 2 * (n - 1);

    // Phase accumulator in "steps * 1000" units
    // Each ms we advance by stepsPerCycle; every time we cross 1000, we do one step.
    static uint32_t acc = 0;
    static uint32_t lastMs = 0;

    uint32_t now = millis();
    uint32_t dt = now - lastMs;
    lastMs = now;

    acc += dt * stepsPerCycle;

    static uint8_t pos = 0;
    static int8_t dir = +1;
    static uint8_t hue = 85;

    // Advance as many steps as needed (handles occasional long dt cleanly)
    while (acc >= 1000) {
        acc -= 1000;

        // move one step along the bounce path
        if (dir > 0) {
            if (pos >= n - 1) { dir = -1; pos--; }
            else pos++;
        } else {
            if (pos == 0)     { dir = +1; pos++; }
            else pos--;
        }

        if (rainbow) hue++; // optional: color shifts slowly each step
    }

    // Render (simple head-only looks best on 4 LEDs)
    for (uint8_t i = 0; i < n; i++) ledsSetPixelPacked(i, 0);
    ledsSetPixelPacked(pos, colorHSV8(rainbow ? hue : 0, 255, 255));
    ledsShow();
}

void ledBinaryCounter(uint16_t updateDelayTime)
{
    static uint32_t lastTick = 0;
    uint32_t now = millis();

    // 500 ms tick (2 Hz)
    if (now - lastTick < updateDelayTime) return;
    lastTick += 500;   // prevents drift

    static uint8_t counter = 0;
    counter++;         // wraps naturally at 255, but we’ll mask bits

    // Render bits
    for (uint8_t i = 0; i < LED_NUM_PIXELS; i++) {
        bool bitOn = counter & (1 << i);
        uint8_t led = (LED_NUM_PIXELS - 1) - i;
        if (bitOn) {
            ledsSetPixelPacked(led, colorHSV8(baseHue, 255, 255)); // ON
        } else {
            ledsSetPixel(led, 0, 0, 0);                     
        }
    }
    baseHue++;
    ledsShow();
}
