#include <stdint.h>
#include "config.h"
#include "logger/logger.h"
#include "ldr/ldr.h"
#include "leds.h"
#include "time/timeReporter.h"

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

  uint32_t color = colorHSV8(baseHue, 255, 255); //hue from 0-255, saturation 0-255, value 0-255
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

void ledBinaryCounter()
{   
    static uint32_t lastSec = -1;

    uint32_t secSinceMidnight = timeReporter_secondsSinceMidnight();
    uint8_t currentSec = secSinceMidnight % 60;
   

    if(currentSec == lastSec) return;
    lastSec = currentSec;
    
    uint8_t currentQuarter = currentSec / 15;
    uint8_t remainderSec = currentSec % 15;

    static const uint8_t quarterHue[4] = {
        //map colours in order they appear on the rainbow to represent minute quarters
        85,   // red
        0,    // green
        190,  // cyan
        128   // indigo
    };

    uint8_t hue = quarterHue[currentQuarter];

    // Render bits
    for (uint8_t i = 0; i < LED_NUM_PIXELS; i++) {
        bool bitOn = remainderSec & (1 << i);
        uint8_t led = (LED_NUM_PIXELS - 1) - i;
        if (bitOn) {
            ledsSetPixelPacked(led, colorHSV8(hue, 255, 255)); // ON
        } else {
            ledsSetPixelPacked(led, colorHSV8(hue, 255, 20));                     
        }
    }
    ledsShow();
}

// Call this from updateLeds() when this mode is active.
void ledPulseShockwave()
{
    static int32_t lastSec = -1;
    static uint32_t pulseStartMs = 0;

    static uint8_t epicenter = 0;
    static uint8_t priorEpicenter = 0;
    //static uint8_t baseHue = 0;

    uint32_t now = millis();

    int32_t secMid = timeReporter_secondsSinceMidnight();
    if (secMid < 0) return; // time not valid yet

    // Start a new pulse on second change (not millisecond-accurate, and that's fine)
    if (secMid != lastSec) {
        lastSec = secMid;
        pulseStartMs = now;

        epicenter = (priorEpicenter + (uint8_t)random(1, LED_NUM_PIXELS)) % LED_NUM_PIXELS;
        priorEpicenter = epicenter;

        baseHue = (uint8_t)(baseHue + 25);
    }

    uint32_t t = now - pulseStartMs;

    // Pulse active for first 500 ms

    const uint16_t ringDelayMs = 150;
    const uint16_t fadeMs      = 250;

    const uint16_t pulseDurationMs =
    ringDelayMs * 3 + fadeMs;
    if (t >= pulseDurationMs) {
        ledsClear();
        ledsShow();
        return;
    }

    

    auto ringBrightness = [&](uint8_t ringIndex) -> uint8_t {
        int32_t dt = (int32_t)t - (int32_t)(ringIndex * ringDelayMs);
        if (dt < 0 || dt >= (int32_t)fadeMs) return 0;

        uint16_t half = fadeMs / 2;
        uint8_t v;
        if ((uint16_t)dt <= half) v = (uint8_t)((uint32_t)dt * 255u / half);
        else {
            uint16_t down = (uint16_t)dt - half;
            v = (uint8_t)(255u - ((uint32_t)down * 255u / half));
        }
        return v;
    };

    ledsClear();

    for (uint8_t i = 0; i < LED_NUM_PIXELS; i++) {
        uint8_t dist = (i > epicenter) ? (i - epicenter) : (epicenter - i); // straight line

        if (dist > 3) continue; // include your optional dist==3 ring

        uint8_t v = ringBrightness(dist);

        if (dist == 1) v = (uint8_t)((uint16_t)v * 180 / 255);
        if (dist == 2) v = (uint8_t)((uint16_t)v * 120 / 255);
        if (dist == 3) v = (uint8_t)((uint16_t)v * 70  / 255);

        if (v == 0) continue;

        ledsSetPixelPacked(i, colorHSV8(baseHue, 255, v));
    }

    ledsShow();
}
