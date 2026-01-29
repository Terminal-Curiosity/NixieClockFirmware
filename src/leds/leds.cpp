#include "leds.h"
#include "animations.h"
#include "config.h"
#include "logger/logger.h"
#include "ldr/ldr.h" 
#include "tetris.h"
#include "time/timeReporter.h"


static constexpr uint8_t BRIGHTNESS_THRESHOLD = 20;
static Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_NUM_PIXELS, ledStringDin, NEO_GRB + NEO_KHZ800);

static uint8_t previousBrightness = 1;
static uint8_t currentBrightness = 1;

void brightnessDetectUpdate();
static void runTetrisDemo();
static void runTetrisGame();
static LedMode ledSchedulerChooseMode();
static void ledModeEnter(LedMode);
static void runLedMode(LedMode);
static const char* ledModeToString(LedMode);

static LedMode currentLedMode = LEDMODE_TETRIS_DEMO;
static LedMode previousLedMode = LEDMODE_OFF;

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
//main LED update function
  {
  brightnessDetectUpdate();  // change LED brightness pending room brightness

  //currentLedMode = ledSchedulerChooseMode();
    currentLedMode = LEDMODE_BINARY_COUNTER;

  if (currentLedMode != previousLedMode) {
    ledModeEnter(currentLedMode);
    logInfo("LED mode changed: %s", ledModeToString(currentLedMode));
    previousLedMode = currentLedMode;
  }

  runLedMode(currentLedMode);
}

void brightnessDetectUpdate()
{
  static constexpr uint8_t hysteresisOffset = 5;
  uint8_t ldr = ldrReportValueAsPercentage();

    if(ldr < BRIGHTNESS_THRESHOLD - hysteresisOffset)
  {
    currentBrightness = 50;
  }
  else if(ldr > BRIGHTNESS_THRESHOLD + hysteresisOffset)
  {
    currentBrightness = 255;
  }

  if(currentBrightness != previousBrightness)
  {
     strip.setBrightness(currentBrightness);

     const char* msg = (currentBrightness <= 50) ? "Dim" : "Bright";
     logInfo("LED brightness changed: %s ", msg);
     logInfo("LED brightness value: %u", currentBrightness);
     previousBrightness = currentBrightness;
  }
}


void setFourPixelsEqual(uint32_t color) {
  strip.setPixelColor(0, color);  
  strip.setPixelColor(1, color);
  strip.setPixelColor(2, color);
  strip.setPixelColor(3, color);
  strip.show();
}

void ledsSetPixel(uint8_t i, uint8_t r, uint8_t g, uint8_t b)
{
  strip.setPixelColor(i, r, g, b);
}

uint32_t ledsColour(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}


void ledsSetPixelPacked(uint8_t i, uint32_t RGB)
{
  strip.setPixelColor(i, RGB);
}

void ledsShow()
{
  strip.show();
}

void ledsClear()
{
  strip.setPixelColor(0, 0);  
  strip.setPixelColor(1, 0);
  strip.setPixelColor(2, 0);
  strip.setPixelColor(3, 0);
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

static void runTetrisDemo()
{
  static bool initialized = false;
  static uint32_t gameOverSince = 0;

  if (!initialized) {
    ledTetrisReset();
    initialized = true;
    gameOverSince = 0;
  }

  LedTetrisState s = ledTetrisUpdate();  // pick a default speed for now

  if (s.gameOver) {
    if (gameOverSince == 0) gameOverSince = millis();

    // Wait long enough for blink+off to complete, plus a pause
    if (millis() - gameOverSince >= 6000) {
      ledTetrisReset();
      gameOverSince = 0;
    }
  } else {
    gameOverSince = 0;
  }
}

static void runTetrisGame()
{
  static bool initialized = false;

  if (!initialized) {
    ledTetrisReset();
    initialized = true;
  }

  LedTetrisState s = ledTetrisUpdate();

  // If s.gameOver becomes true, it will blink+off and then stay off.
  // use s.score to print score on nixies
}

static LedMode ledSchedulerChooseMode()
{

 int32_t sec = timeReporter_secondsSinceMidnight();
  if (sec < 0) {
    // Time not valid yet (pre-NTP). Pick something safe.
    return   LEDMODE_OFF;
  }

  if (sec < 7*3600)
  //if time is between midnight and 7am, disable LEDs
  return LEDMODE_OFF;

  static int8_t lastHour = -1;
  uint8_t currentIndex = 0;
  
  uint8_t hour = (int8_t)(sec / 3600);

  if(hour != lastHour)
  {
    lastHour = hour;
    if(LEDMODE_ENUM_COUNT > 1){
    // make sure there's more than 1 option to choose from 
     uint8_t next;
      do {
        next = (uint8_t)random(0, LEDMODE_ENUM_COUNT);
      } while (next == currentIndex); // optional: no immediate repeat
      currentIndex = next;
    } else {
      currentIndex = 0;

  }
  }
  return static_cast<LedMode>(currentIndex);


}

static void ledModeEnter(LedMode m)
{
  // Do one-time init per mode
  switch (m) {
    case LEDMODE_TETRIS_DEMO:
      ledTetrisReset();
      break;
    case LEDMODE_TETRIS_GAME:
      ledTetrisReset();
      break;
    default:
      break;
  }
}

static void runLedMode(LedMode m)
{
  switch (m) {
    case LEDMODE_OFF:
      ledsClear();
      ledsShow();
      break;

    case LEDMODE_RAINBOW_FADE:
      ledRainbowFade();        
      break;

    case LEDMODE_RAINBOW_WAVE:
      ledRainbowWave();        
      break;

    case LEDMODE_NIGHTRIDER:
      ledNightRider(true);     
      break;

    case LEDMODE_PULSE_SHOCKWAVE:
      ledPulseShockwave();     
      break;

    case LEDMODE_BINARY_COUNTER:
      ledBinaryCounter();
      break;

    case LEDMODE_TETRIS_DEMO:
      runTetrisDemo();
      break;

    default:
      break;
  }
}

static const char* ledModeToString(LedMode mode)
{
    switch (mode) {
        case LEDMODE_OFF:            return "OFF";
        case LEDMODE_RAINBOW_WAVE:   return "RAINBOW_WAVE";
        case LEDMODE_RAINBOW_FADE:   return "RAINBOW_FADE";
        case LEDMODE_NIGHTRIDER:     return "NIGHTRIDER";
        case LEDMODE_BINARY_COUNTER: return "BINARY_COUNTER";
        case LEDMODE_PULSE_SHOCKWAVE: return "PULSE_SHOCKWAVE";
        case LEDMODE_TETRIS_DEMO:    return "TETRIS_DEMO";
        case LEDMODE_TETRIS_GAME:    return "TETRIS_GAME";
        default:                     return "UNKNOWN";
    }
}