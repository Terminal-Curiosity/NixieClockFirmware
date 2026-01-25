#include "ldr.h"
#include "config.h"
#include "logger/logger.h"

static uint16_t ldrValue=0;
int current_brightness_percent = MAX_BRIGHTNESS_PERCENT;
static constexpr uint32_t LDR_READ_DELAY = 1000; //add a delay for reading the LDR to prevent spam 
static uint32_t lastReadMs = 0;

bool ldrInit(void)
{
    pinMode(LDR_pin, INPUT);  
    analogSetAttenuation(ADC_11db); // sets attenuation of LDR input (otherwise it will be 0-1V)
    logInfo("LDR Initialized.");
    return true;
}

void ldrReadSave()
{
    uint32_t nowMs = millis();
    if ((int32_t)(nowMs - lastReadMs) < LDR_READ_DELAY)
      return;

    ldrValue = analogRead(LDR_pin); 
    lastReadMs = nowMs;
    //logInfo("LDR value: %i",ldrReportValueAsPercentage());
}

uint16_t ldrReportRawValue()
{
    return ldrValue;
}

uint8_t ldrReportValueAsPercentage() {
  // Maps the raw ADC value (from LDR_BRIGHT_ADC_MIN to LDR_DARK_ADC_MAX)
  // to the desired brightness percentage (from MAX_BRIGHTNESS_PERCENT to MIN_BRIGHTNESS_PERCENT).
  // Remember to calibrate LDR_BRIGHT_ADC_MIN and LDR_DARK_ADC_MAX for your specific setup.

  current_brightness_percent = map(ldrReportRawValue(), 
                                   LDR_BRIGHT_ADC_MIN,  // Input lower bound (ADC val for bright)
                                   LDR_DARK_ADC_MAX,    // Input upper bound (ADC val for dark)
                                   MAX_BRIGHTNESS_PERCENT, // Output lower bound (for bright)
                                   MIN_BRIGHTNESS_PERCENT  // Output upper bound (for dark)
                                  );

  // Ensures the brightness value stays within the defined min/max percentage.
  current_brightness_percent = constrain(current_brightness_percent, 
                                         MIN_BRIGHTNESS_PERCENT, 
                                         MAX_BRIGHTNESS_PERCENT
                                        );
  return current_brightness_percent;
}
