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
    const uint16_t stepsPerCycle = 2 * (LED_NUM_PIXELS - 1);

    static int32_t  lastSecMid   = -1;
    static uint32_t secStartMs   = 0;

    static uint16_t stepCounter  = 0;   // counts micro-steps since :00
    static uint8_t  lastStep     = 0xFF;

    uint32_t now = millis();

    int32_t secMid = timeReporter_secondsSinceMidnight();
    if (secMid < 0) return;

    // Reset at top of minute (do this once per minute)
    if (secMid != lastSecMid) {
        lastSecMid = secMid;
        secStartMs = now;

        if ((secMid % 60) == 0) {
            stepCounter = 0;
            lastStep = 0xFF; // ok to keep; we'll handle counting safely below
        }
    }

    uint32_t phaseMs = now - secStartMs;
    if (phaseMs >= 1000) phaseMs = 999;

    uint8_t step = (uint8_t)((phaseMs * stepsPerCycle) / 1000);

    // Total micro-steps per minute (n=4 => 360)
    const uint16_t STEPS_PER_MIN = 60 * stepsPerCycle;

    // Count a "pulse" whenever the step changes
    if (step != lastStep) {
        lastStep = step;

        // Keep it in-range and perfectly periodic
        stepCounter++;
        if (stepCounter >= STEPS_PER_MIN) stepCounter = 0;   // <<< key
    }

    uint8_t pos = (step <= (LED_NUM_PIXELS - 1)) ? step : (stepsPerCycle - step);
    uint8_t ledIndex = (LED_NUM_PIXELS - 1) - pos;

    // Full wheel per minute, starting at your "red" hue
    const uint8_t HUE_START = 85;   // your red
    const uint8_t HUE_SPAN  = 255;  // almost full wheel (smooth seam)

    uint8_t hue = HUE_START;
    if (rainbow) {
        
        uint8_t ramp = (uint8_t)(((uint32_t)stepCounter * HUE_SPAN) / STEPS_PER_MIN);
        hue = (uint8_t)(HUE_START - ramp);
    }

    ledsClear();
    uint8_t headHue = rainbow ? hue : 85;
    ledsSetPixelPacked(ledIndex, colorHSV8(headHue, 255, 255));
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

// Heartbeat animation for 4 LEDs (non-blocking)
// - "Lub-dub" double beat
// - Starts on a real second change (using timeReporter_secondsSinceMidnight())
// - Optional: hue can drift slowly, or you can keep it fixed
//
// Depends on your helpers:
//   uint32_t colorHSV8(uint8_t h, uint8_t s, uint8_t v);
//   void setFourPixelsEqual(uint32_t packedColor);   // or replace with your per-pixel setter
//   void ledsShow();
//   void ledsClear();



static uint8_t hbGamma8(uint8_t x)
{
  // cheap gamma-ish curve: (x^2)/255
  return (uint16_t(x) * uint16_t(x) + 255) >> 8;
}

static uint8_t hbPulse(uint32_t t_ms, uint16_t attack_ms, uint16_t decay_ms)
{
  // returns 0..255
  uint32_t total = (uint32_t)attack_ms + (uint32_t)decay_ms;
  if (t_ms >= total) return 0;

  uint16_t a = attack_ms;
  uint16_t d = decay_ms;

  uint8_t level;
  if (t_ms < a) {
    // linear attack 0->255
    level = (uint8_t)((t_ms * 255u) / a);
  } else {
    // linear decay 255->0
    uint32_t td = t_ms - a;
    level = (uint8_t)(255u - ((td * 255u) / d));
  }
  return hbGamma8(level);
}

void ledHeartbeat(uint8_t hueFixed, bool hueDrift)
{
    static int32_t  lastSecMid = -1;
    static uint32_t secStartMs = 0;
    static uint8_t  hue = 0;
    static int16_t beatJitter = 0;


    uint32_t now = millis();

    int32_t secMid = timeReporter_secondsSinceMidnight();
    if (secMid < 0) return;

    if (secMid != lastSecMid) {
        lastSecMid = secMid;
        secStartMs = now;
        hue = hueDrift ? (uint8_t)(hue + 5) : hueFixed;
        // ±20 ms feels organic without being obvious
        beatJitter = random(-30, 31);
    }

    uint32_t t = now - secStartMs;

    // Heartbeat timing (lub-dub)
const uint16_t lubAttack = 35 + beatJitter / 4;
const uint16_t lubDecay  = 220 + beatJitter / 2;
const uint16_t gapMs     = 120 + beatJitter / 3;
const uint16_t dubAttack = 25 + beatJitter / 4;
const uint16_t dubDecay  = 200 + beatJitter / 2;


    const uint32_t dubStart = (uint32_t)lubAttack + lubDecay + gapMs;

    uint8_t env = hbPulse(t, lubAttack, lubDecay);
    if (t >= dubStart) {
        uint8_t dubEnv = hbPulse(t - dubStart, dubAttack, dubDecay);
        if (dubEnv > env) env = dubEnv;
    }

    // Baseline + peak control
    const uint8_t baseDim = 2;
    const uint8_t maxV    = 140;

    uint16_t scaledInner = (uint16_t)env * maxV / 255u;
    uint8_t vInner = (scaledInner > baseDim) ? scaledInner : baseDim;

    // Outer LEDs at ~half brightness
    const uint8_t outerScale = 90; // ~50%
    uint16_t scaledOuter = (uint16_t)vInner * outerScale / 255u;
    uint8_t vOuter = (scaledOuter > baseDim) ? scaledOuter : baseDim;

    // Render
    ledsClear();

    uint32_t cInner = colorHSV8(hue, 255, vInner);
    uint32_t cOuter = colorHSV8(hue, 255, vOuter);

    // Outer LEDs
    ledsSetPixelPacked(0, cOuter);
    ledsSetPixelPacked(3, cOuter);

    // Inner LEDs (the "heart")
    ledsSetPixelPacked(1, cInner);
    ledsSetPixelPacked(2, cInner);

    ledsShow();
}

