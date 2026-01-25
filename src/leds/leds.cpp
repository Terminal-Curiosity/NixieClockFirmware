#include "leds.h"
#include "config.h"
#include "logger/logger.h"

static Adafruit_NeoPixel strip = Adafruit_NeoPixel(4, ledStringDin, NEO_GRB + NEO_KHZ800);

static uint16_t hue = 0;
static uint32_t color = colorHSV(hue, 1.0, 1.0); //hue from 0-360, saturation 0-1, value 0-1

bool ledsInit(void) {
    pinMode(ledStringDin,OUTPUT);
    strip.begin();              // Initialize NeoPixel strip
    strip.setBrightness(255);   // set maximum brightness
    strip.show();               // Turn off all LEDs at start
    logInfo("LEDs Initialized.");
    return true;
}

void updateLeds()
//generic update function with  future capability to call many different led effects 
{
  ledSlowRainbowFade();
}

void ledSlowRainbowFade(uint16_t updateDelayTime) {

  static uint32_t lastUpdateMs = 0;
  const uint16_t updateIntervalMs = updateDelayTime; // Update rainbow colour every n milliseconds
  uint32_t currentMs = millis();

  if (currentMs - lastUpdateMs < updateIntervalMs) {
    return; // Not enough time has passed, exit the function
  }

  lastUpdateMs = currentMs;
    setFourPixelsEqual(color);
    hue = (hue + 1) % 360;
    color = colorHSV(hue, 1.0, 1.0);
}

void setFourPixelsEqual(uint32_t color) {
  strip.setPixelColor(0, color);  
  strip.setPixelColor(1, color);
  strip.setPixelColor(2, color);
  strip.setPixelColor(3, color);
  strip.show();
}

uint32_t colorHSV(float h, float s, float v) {
  float c = v * s;
  float x = c * (1 - fabs(fmod(h / 60.0, 2) - 1));
  float m = v - c;

  float r1, g1, b1;
  if      (h < 60)  { r1 = c; g1 = x; b1 = 0; }
  else if (h < 120) { r1 = x; g1 = c; b1 = 0; }
  else if (h < 180) { r1 = 0; g1 = c; b1 = x; }
  else if (h < 240) { r1 = 0; g1 = x; b1 = c; }
  else if (h < 300) { r1 = x; g1 = 0; b1 = c; }
  else              { r1 = c; g1 = 0; b1 = x; }

  uint8_t r = (r1 + m) * 255;
  uint8_t g = (g1 + m) * 255;
  uint8_t b = (b1 + m) * 255;

  return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}