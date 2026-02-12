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
  for(int i = 0; i < LED_NUM_PIXELS; i++) ledsSetPixelPacked(i, color);

  ledsShow();
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
    const uint8_t HUE_START = 250;   // your red
    const uint8_t HUE_SPAN  = 255;  // almost full wheel (smooth seam)

    uint8_t hue = HUE_START;
    if (rainbow) {
        
        uint8_t ramp = (uint8_t)(((uint32_t)stepCounter * HUE_SPAN) / STEPS_PER_MIN);
        hue = (uint8_t)(HUE_START + ramp);
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
        0,   // red
        85,    // green
        160,  // cyan
        220   // indigo
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
        if (dist == 3) v = (uint8_t)((uint16_t)v * 60  / 255);

        if (v == 0) continue;

        ledsSetPixelPacked(i, colorHSV8(baseHue, 255, v));
    }

    ledsShow();
}


// -------------------- Heartbeat helpers --------------------

static uint8_t triEnv(uint32_t tMs, uint16_t lenMs)
{
    if (tMs >= lenMs) return 0;
    uint16_t half = (uint16_t)(lenMs / 2);
    if (half == 0) return 0;

    if (tMs <= half) {
        return (uint8_t)((tMs * 255u) / half);
    } else {
        uint32_t down = (uint32_t)tMs - half;
        return (uint8_t)(255u - ((down * 255u) / half));
    }
}

// 50..125 BPM with heavy bias around 65..80, occasional excursions
static uint16_t pickBiasedBpm()
{
    uint8_t r = (uint8_t)random(0, 100);

    if (r < 70) {
        return (uint16_t)random(65, 81);      // 65..80 (common)
    } else if (r < 92) {
        return (uint16_t)random(60, 90);      // 60..90 (moderate wander)
    } else if (r < 96) {
        return (uint16_t)random(50, 60);      // 50..60 (rare low)
    } else {
        return (uint16_t)random(90, 125);     // 90..125 (rare high)
    }
}

// BPM -> Hue (piecewise low->mid->high), with a “mood mapping” toggle.
static uint8_t hueFromBpmQ8(int32_t bpmQ8)
{
    const uint8_t HUE_RED   = 00;
    const uint8_t HUE_GREEN = 85;
    const uint8_t HUE_BLUE  = 170;

    const int32_t BPM_MIN_Q8  = 50  * 256;
    const int32_t BPM_PIV_Q8  = 75  * 256;   // your "common" heartbeat point
    const int32_t BPM_MAX_Q8  = 140 * 256;

    // Clamp bpmQ8 into 50..125
    if (bpmQ8 < BPM_MIN_Q8) bpmQ8 = BPM_MIN_Q8;
    if (bpmQ8 > BPM_MAX_Q8) bpmQ8 = BPM_MAX_Q8;

    
    if (bpmQ8 < BPM_PIV_Q8) {
        uint32_t spanQ8 = (uint32_t)(BPM_PIV_Q8 - BPM_MIN_Q8);      // 25*256
        uint32_t xQ8    = (uint32_t)(bpmQ8     - BPM_MIN_Q8);      // 0..spanQ8
        uint16_t t      = (uint16_t)((xQ8 * 255u) / (spanQ8 ? spanQ8 : 1)); // 0..255

        int16_t dh = (int16_t)HUE_GREEN - (int16_t)HUE_BLUE;       // -85
        return (uint8_t)((int16_t)HUE_BLUE + (int16_t)(dh * t) / 255);
    }

    // Above pivot: GREEN -> RED across 75..125 (50 BPM span)
    {
        uint32_t spanQ8 = (uint32_t)(BPM_MAX_Q8 - BPM_PIV_Q8);      // 50*256
        uint32_t xQ8    = (uint32_t)(bpmQ8     - BPM_PIV_Q8);      // 0..spanQ8
        uint16_t t      = (uint16_t)((xQ8 * 255u) / (spanQ8 ? spanQ8 : 1)); // 0..255

        int16_t dh = (int16_t)HUE_RED - (int16_t)HUE_GREEN;        // -85
        return (uint8_t)((int16_t)HUE_GREEN + (int16_t)(dh * t) / 255);
    }

}

// -------------------- heartbeat --------------------

void ledHeartbeat()
{
    // --- timing base ---
    static uint32_t lastMs = 0;
    uint32_t now = millis();
    uint32_t dt = now - lastMs;
    lastMs = now;
    if (dt > 60) dt = 60;

    // --- Smooth BPM wander (Q8 fixed-point) ---
    static bool seeded = false;
    static int32_t bpmQ8 = 75 * 256;
    static int32_t targetQ8 = 75 * 256;
    static uint32_t targetHoldMs = 0;

    if (!seeded) {
        int32_t r = (int32_t)random(-5, 6);
        bpmQ8 = (75 + r) * 256;
        targetQ8 = bpmQ8;
        targetHoldMs = 0;
        seeded = true;
    }

    // choose new target every 2..8 s, but move toward it smoothly
    if (targetHoldMs <= dt) {
        targetHoldMs = (uint32_t)random(2000, 8001);
        targetQ8 = (int32_t)pickBiasedBpm() * 256;
    } else {
        targetHoldMs -= dt;
    }

    const int32_t smoothFactor = 80; // larger = slower flow
    bpmQ8 += (targetQ8 - bpmQ8) / smoothFactor;

    // Clamp 50..125
    if (bpmQ8 < 50 * 256)  bpmQ8 = 50 * 256;
    if (bpmQ8 > 140 * 256) bpmQ8 = 140 * 256;

    // Correct BPM -> period conversion: 60000 ms per minute
    uint32_t periodMs = (60000u * 256u) / (uint32_t)bpmQ8;

    // Hue follows BPM (smoothly, no per-second stepping)
    uint8_t hueBase = hueFromBpmQ8(bpmQ8);

    // Optional: tiny hue shift on dub to help the eye read direction (can comment out)
    uint8_t hueLub = hueBase;
    uint8_t hueDub = (uint8_t)(hueBase + 8);

    // --- Heartbeat phases: lub, gap, dub, rest ---
    static uint32_t phaseStartMs = 0;
    static uint8_t  phase = 0; // 0=lub, 1=gap, 2=dub, 3=rest

    uint16_t lubLen  = (uint16_t)(periodMs / 8);
    uint16_t gapLen  = (uint16_t)(periodMs / 16);
    uint16_t dubLen  = (uint16_t)(periodMs / 10);

    if (lubLen < 40) lubLen = 40;
    if (dubLen < 35) dubLen = 35;
    if (gapLen < 20) gapLen = 20;

    uint32_t t = now - phaseStartMs;

    if (phase == 0) {
        if (t >= lubLen) { phase = 1; phaseStartMs = now; t = 0; }
    } else if (phase == 1) {
        if (t >= gapLen) { phase = 2; phaseStartMs = now; t = 0; }
    } else if (phase == 2) {
        if (t >= dubLen) { phase = 3; phaseStartMs = now; t = 0; }
    } else {
        uint32_t used = (uint32_t)lubLen + gapLen + dubLen;
        uint32_t restLen = (periodMs > used) ? (periodMs - used) : 0;
        if (t >= restLen) { phase = 0; phaseStartMs = now; t = 0; }
    }

    // Envelope for current phase at time tt
    auto envAt = [&](uint32_t tt) -> uint8_t {
        if (phase == 0) {
            return triEnv(tt, lubLen);
        } else if (phase == 2) {
            uint8_t dub = triEnv(tt, dubLen);
            return (uint8_t)((uint16_t)dub * 180u / 255u); // dub weaker
        } else {
            return 0;
        }
    };

    // -------------------- direction / propagation --------------------
    // Lub propagates outward from LED 1, Dub propagates outward from LED 2.

    // Visible-but-tasteful propagation (per hop)
    uint16_t basePropMs = (uint16_t)(periodMs / 20); // ~2.8% of period per hop
    if (basePropMs < 20) basePropMs = 20;
    if (basePropMs > 70) basePropMs = 70;

    // Phase origin: lub -> 1, dub -> 2
    uint8_t origin = (phase == 2) ? 2 : 1;

    // -------------------- render --------------------
    ledsClear();

    for (uint8_t i = 0; i < LED_NUM_PIXELS; i++) {

        // hop distance from origin (0,1,2)
        uint8_t dist = (i > origin) ? (i - origin) : (origin - i);
        uint16_t delayMs = (uint16_t)(dist * basePropMs);

        uint8_t bri = 0;
        if (t > delayMs) bri = envAt(t - delayMs);

        // Outer LEDs dimmer to keep the "heart core" feel
        if (i == 0 || i == 3) bri = (uint8_t)(bri >> 1);

        if (bri == 0) continue;

        uint8_t useHue = (phase == 2) ? hueDub : hueLub;

        // Flip index if needed for your physical strip
        uint8_t led = (LED_NUM_PIXELS - 1) - i; // remove if not needed
        ledsSetPixelPacked(led, colorHSV8(useHue, 255, bri));
        
    }

    ledsShow();
}
