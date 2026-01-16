#include "ldr.h"
#include "config.h"

static uint8_t ldrValue=0;
int current_brightness_percent = MAX_BRIGHTNESS_PERCENT;

void ldrInit()
{
    pinMode(LDR_pin, INPUT);  
    analogSetAttenuation(ADC_11db); // sets attenuation of LDR input (otherwise it will be 0-1V)
}

void ldrReadSave()
{
    ldrValue = analogRead(LDR_pin); 
}

uint8_t ldrReportRawValue()
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