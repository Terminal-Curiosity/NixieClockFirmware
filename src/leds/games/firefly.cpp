#include "../animations.h"
#include "config.h"

void ledFireflySync(uint16_t basePeriodMs)
{
    static bool seeded = false;

    // Per-firefly state
    static uint16_t period[LED_NUM_PIXELS];      // ms per cycle
    static uint16_t phase[LED_NUM_PIXELS];       // 0..period-1
    static uint16_t flashLeft[LED_NUM_PIXELS];   // ms remaining in flash (uint16_t = safer)

    // Simple per-firefly colour palette (fill/tune later)
    static uint8_t huePal[LED_NUM_PIXELS] = {
        85,   // red-ish
        140,  // purple-ish
        200,  // cyan-ish
        30    // green
    };

    static uint32_t lastMs = 0;

    // Tuning knobs
    const uint16_t FLASH_MS      = 100;    // flash duration
    const uint8_t  DIM_V         = 10;    // idle glow
    const uint8_t  FLASH_V       = 255;   // flash brightness

    const uint16_t PERIOD_JITTER = 400;   // bigger -> harder to instantly sync
    const uint16_t PHASE_KICK    = 30;    // smaller -> slower syncing

    const uint16_t COUPLE_WINDOW = 450;   // only nudge if within last ~X ms of cycle
    const uint8_t  COUPLE_PROB   = 60;    // % chance to respond to a neighbor flash

    uint32_t now = millis();
    uint32_t dt = now - lastMs;
    lastMs = now;

    // Clamp dt so long stalls don’t teleport phases
    if (dt > 80) dt = 80;

    if (!seeded) {
        for (uint8_t i = 0; i < LED_NUM_PIXELS; i++) {
            period[i] = (uint16_t)(basePeriodMs + random(0, PERIOD_JITTER));
            phase[i]  = (uint16_t)random(0, period[i]);
            flashLeft[i] = 0;
        }
        seeded = true;
    }

    // Step 1: advance phases, handle flashes
    bool flashedThisFrame[LED_NUM_PIXELS] = {false, false, false, false};

    for (uint8_t i = 0; i < LED_NUM_PIXELS; i++) {

        // Count down flash timer
        if (flashLeft[i] > 0) {
            if (flashLeft[i] > dt) flashLeft[i] -= (uint16_t)dt;
            else flashLeft[i] = 0;
        }

        // Advance phase
        uint32_t p = (uint32_t)phase[i] + dt;

        if (p >= period[i]) {
            // Flash!
            flashedThisFrame[i] = true;
            flashLeft[i] = FLASH_MS;

            // Wrap phase
            p -= period[i];

            // Slightly vary this firefly’s period over time
            int16_t jitter = (int16_t)random(-25, 26);
            int32_t newP = (int32_t)period[i] + jitter;
            if (newP < 900)  newP = 900;
            if (newP > 2400) newP = 2400;
            period[i] = (uint16_t)newP;
        }

        phase[i] = (uint16_t)p;
    }

    // Step 2: coupling — only nudge those already near flashing, and not always
    for (uint8_t j = 0; j < LED_NUM_PIXELS; j++) {
        if (!flashedThisFrame[j]) continue;

        for (uint8_t i = 0; i < LED_NUM_PIXELS; i++) {
            if (i == j) continue;
            if (flashLeft[i] > 0) continue;          // ignore if currently flashing

            // Random chance to respond (makes it feel alive)
            if ((uint8_t)random(0, 100) >= COUPLE_PROB) continue;

            uint16_t p   = phase[i];
            uint16_t per = period[i];

            // Only respond if close to end of cycle (prevents instant lock)
            uint16_t toGo = (uint16_t)(per - 1 - p);
            if (toGo > COUPLE_WINDOW) continue;

            uint32_t bumped = (uint32_t)p + PHASE_KICK;
            if (bumped >= per) bumped = per - 1;
            phase[i] = (uint16_t)bumped;
        }
    }

    // Step 3: render
    ledsClear();

    for (uint8_t i = 0; i < LED_NUM_PIXELS; i++) {
        uint8_t v = (flashLeft[i] > 0) ? FLASH_V : DIM_V;

        // “heart” brightness: inner brighter
        if (flashLeft[i] == 0) {
            if (i == 1 || i == 2) v = (uint8_t)(DIM_V + 6);
        } else {
            if (i == 1 || i == 2) v = FLASH_V;
            else v = (uint8_t)(FLASH_V / 2);
        }

        uint8_t led = (LED_NUM_PIXELS - 1) - i; // remove if not needed
        ledsSetPixelPacked(led, colorHSV8(huePal[i], 220, v));
    }

    ledsShow();
}
