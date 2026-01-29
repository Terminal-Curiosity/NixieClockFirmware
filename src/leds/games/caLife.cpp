#include "caLife.h"
#include "leds/animations.h"
#include "time/timeReporter.h"

#ifndef LED_NUM_PIXELS
#define LED_NUM_PIXELS 4
#endif

// Cyclic Cellular Automaton (CCA)
// Each cell has a state 0..K-1 (this IS the colour index).
// If a neighbor is (state+1)%K, the cell advances after inertia.
// Includes:
//  - cell inertia (pressure)
//  - rare spark injection
//  - slow breathing brightness envelope

// Number of discrete colours
static const uint8_t K = 7;

/*
 * Calibrated hue palette (FILL THIS IN LATER)
 *
 * Order defines the direction of colour flow.
 * You said red ≈ 85.
 *
 * Example placeholders:
 *
 * static const uint8_t huePal[K] = {
 *     85,   // red
 *     ?,    // orange
 *     ?,    // yellow
 *     ?,    // green
 *     ?,    // cyan
 *     ?,    // blue
 *     ?,    // purple
 * };
 */
static uint8_t huePal[K] = {
    85,  // placeholder red
    49,
    13,
    10,
    233,
    196,
    128
};

static uint8_t clampNeighborL(int8_t i)
{
    return (i < 0) ? 0xFF : (uint8_t)i;
}

static uint8_t clampNeighborR(int8_t i)
{
    return (i >= LED_NUM_PIXELS) ? 0xFF : (uint8_t)i;
}

static uint8_t ca_randomStateK()
{
    return (uint8_t)random(0, K);
}

void ledCyclicColorCA(uint16_t updateMs)
{
    static uint32_t lastMs = 0;
    static bool seeded = false;

    static uint8_t s[LED_NUM_PIXELS];               // colour indices 0..K-1
    static uint8_t pressure[LED_NUM_PIXELS] = {0};  // inertia per cell
    static uint8_t boringCount = 0;

    // breathing state
    static uint16_t breathe = 0;

    uint32_t now = millis();
    if (now - lastMs < updateMs) return;
    lastMs = now;

    if (!seeded) {
        int32_t secMid = timeReporter_secondsSinceMidnight();
        uint8_t seed = (secMid < 0) ? 0 : (uint8_t)(secMid % K);

        // Seed with a small gradient so we don't freeze immediately
        for (uint8_t i = 0; i < LED_NUM_PIXELS; i++) {
            s[i] = (uint8_t)((seed + i) % K);
            pressure[i] = 0;
        }

        boringCount = 0;
        breathe = 0;
        seeded = true;
    }

    uint8_t next[LED_NUM_PIXELS];
    bool anyChange = false;

    // --- Cell inertia update ---
    const uint8_t INERTIA = 2; // 1=twitchy, 2=sweet spot, 3=slow/organic

    for (uint8_t i = 0; i < LED_NUM_PIXELS; i++) {
        uint8_t C = s[i];
        uint8_t want = (uint8_t)((C + 1) % K);

        uint8_t Li = clampNeighborL((int8_t)i - 1);
        uint8_t Ri = clampNeighborR((int8_t)i + 1);

        bool influenced = false;
        if (Li != 0xFF && s[Li] == want) influenced = true;
        if (Ri != 0xFF && s[Ri] == want) influenced = true;

        if (influenced) {
            if (pressure[i] < 255) pressure[i]++;
        } else {
            pressure[i] = 0;
        }

        if (pressure[i] >= INERTIA) {
            next[i] = want;
            pressure[i] = 0;
            anyChange = true;
        } else {
            next[i] = C;
        }
    }

    // --- Boredom detection + spark ---
    if (anyChange) {
        boringCount = 0;
    } else {
        if (boringCount < 255) boringCount++;
    }

    // Rare dramatic event: inject a colour spark
    const uint8_t BORING_THRESHOLD = 22;
    if (boringCount >= BORING_THRESHOLD) {
        uint8_t idx = (uint8_t)random(0, LED_NUM_PIXELS);
        next[idx] = ca_randomStateK();
        pressure[idx] = 0;
        boringCount = 0;
    }

    // Commit
    for (uint8_t i = 0; i < LED_NUM_PIXELS; i++) {
        s[i] = next[i];
    }

    // --- Breathing envelope (slow triangle wave) ---
    breathe++;

    uint8_t breathV = (breathe & 0x100)
        ? (uint8_t)(255 - (breathe & 0xFF))
        : (uint8_t)(breathe & 0xFF);

    uint8_t globalV = (uint8_t)(170 + (breathV >> 4)); // ~170..185

    // --- Render ---
    ledsClear();
    for (uint8_t i = 0; i < LED_NUM_PIXELS; i++) {
        uint8_t hue = huePal[s[i]];  // <<< explicit colour index

        uint8_t vInner = globalV;
        uint8_t vOuter = (uint8_t)(globalV >> 1);
        uint8_t v = (i == 1 || i == 2) ? vInner : vOuter;

        uint8_t led = (LED_NUM_PIXELS - 1) - i; // remove if not needed
        ledsSetPixelPacked(led, colorHSV8(hue, 255, v));
    }
    ledsShow();
}
