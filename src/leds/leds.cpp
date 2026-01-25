#include "leds.h"
#include "config.h"
#include "logger/logger.h"
#include "ldr/ldr.h" 

static Adafruit_NeoPixel strip = Adafruit_NeoPixel(4, ledStringDin, NEO_GRB + NEO_KHZ800);
static constexpr uint8_t BRIGHTNESS_THRESHOLD = 20;

void brightnessDetectUpdate();

static uint8_t previousBrightness = 1;
static uint8_t currentBrightness = 1;
static uint8_t baseHue = 0;

bool ledsInit(void) {
    pinMode(ESP_LED,OUTPUT);
    digitalWrite(ESP_LED,LOW);  //explicitly disable ESP onboard LED
    
    pinMode(ledStringDin,OUTPUT);
    strip.begin();              // Initialize NeoPixel strip
    strip.setBrightness(1);     // set minimum brightness
    strip.show();               // Turn off all LEDs at start
    logInfo("LEDs Initialized.");
    return true;
}

void updateLeds()
//generic update function with  future capability to call many different led effects 
{
  brightnessDetectUpdate();


  //ledRainbowFade();
  //ledRainbowWave();
  //ledNightRider(true);
  ledBinaryCounter();
}

void brightnessDetectUpdate()
{
  uint8_t hysteresisOffset = 5;
  static const char* message = "default";

  currentBrightness = ldrReportValueAsPercentage();

  //logInfo("brightness: %u", ldrReportValueAsPercentage());
    if(currentBrightness < BRIGHTNESS_THRESHOLD - hysteresisOffset)
  {
    currentBrightness = 50;
    message = "Dim";
  }
  else if(currentBrightness > BRIGHTNESS_THRESHOLD + hysteresisOffset)
  {
    currentBrightness = 255;
    message = "Bright";
  }

  if(currentBrightness != previousBrightness)
  {
     strip.setBrightness(currentBrightness);
     logInfo("LED brightness changed: %s", message);
     previousBrightness = currentBrightness;
  }
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

  for (uint8_t i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, colorHSV8(baseHue + i*offset, 255, 255));
    }

  strip.show();
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
    for (uint8_t i = 0; i < n; i++) strip.setPixelColor(i, 0);
    strip.setPixelColor(pos, colorHSV8(rainbow ? hue : 0, 255, 255));
    strip.show();
}

void ledBinaryCounter(uint16_t updateDelayTime)
{
    const uint8_t n = strip.numPixels();
    if (n == 0) return;

    static uint32_t lastTick = 0;
    uint32_t now = millis();

    // 500 ms tick (2 Hz)
    if (now - lastTick < updateDelayTime) return;
    lastTick += 500;   // prevents drift

    static uint8_t counter = 0;
    counter++;         // wraps naturally at 255, but we’ll mask bits

    // Render bits
    for (uint8_t i = 0; i < n; i++) {
        bool bitOn = counter & (1 << i);
        uint8_t led = (n - 1) - i;
        if (bitOn) {
            strip.setPixelColor(led, colorHSV8(baseHue, 255, 255)); // ON
        } else {
            strip.setPixelColor(led, 10, 10, 10);                      // OFF
        }
    }
    baseHue++;
    strip.show();
}


void setFourPixelsEqual(uint32_t color) {
  strip.setPixelColor(0, color);  
  strip.setPixelColor(1, color);
  strip.setPixelColor(2, color);
  strip.setPixelColor(3, color);
  strip.show();
}

uint32_t colorHSV8(uint8_t h, uint8_t s, uint8_t v)
{
    if (s == 0) {
        return ((uint32_t)v << 16) | ((uint32_t)v << 8) | v;
    }

    uint8_t region = h / 43;                 // 0..5
    uint8_t remainder = (h - region * 43) * 6; // 0..255

    uint8_t p = (uint16_t)v * (255 - s) / 255;
    uint8_t q = (uint16_t)v * (255 - ((uint16_t)s * remainder) / 255) / 255;
    uint8_t t = (uint16_t)v * (255 - ((uint16_t)s * (255 - remainder)) / 255) / 255;

    uint8_t r, g, b;
    switch (region) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        default:r = v; g = p; b = q; break;
    }

    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}