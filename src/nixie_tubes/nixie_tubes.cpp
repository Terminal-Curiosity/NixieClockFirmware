#include "nixie_tubes.h"
#include "pcf8574/pcf8574.h"
#include "time_keeper/time_keeper.h"

static constexpr uint8_t digit_to_pin_mapping[] = {9,0,8,4,12,2,10,6,14,1}; 
//lookup table to map digit values (0-9) to corresponding nixie digits as per wiring between K155ID1 and PCF8574

void showCurrentTime()
{
  uint8_t hours = getTime() / 3600;
  uint8_t minutes = (getTime() % 3600) / 60;
  uint8_t seconds = getTime() % 60;

  //int16_t displayTime = hours * 100 + minutes; //display HHMM only
  int16_t displayTime = minutes * 100 + seconds; //display MMSS only
  tubesDisplayValue(displayTime);
}

void tubesDisplayValue(int value_to_display = 0)
//update the tube display to show the value passed into the function.
//NOTE: tubes 1 and 3 have non-standard digit wiring, so the upper nibbles must be remapped before being used.
{


  //display upper pair
  uint8_t upper = digit_to_pin_mapping[value_to_display / 1000];
  upper = ((upper & 0b1000) | ((upper & 0b0001) << 2) | ((upper & 0b0110) >> 1)) << 4; //realign bits as per wiring
  uint8_t lower = digit_to_pin_mapping[(value_to_display % 1000) / 100];
  uint8_t data_for_pcf2 = upper | lower;
  pcf8574WriteOneByte(PCF8574_ADDRESS_2, data_for_pcf2); //upper digits

  //display lower pair
  upper = digit_to_pin_mapping[(value_to_display % 100) / 10];
  upper = ((upper & 0b1000) | ((upper & 0b0001) << 2) | ((upper & 0b0110) >> 1)) << 4; //realign bits as per wiring
  lower = digit_to_pin_mapping[(value_to_display % 10)];
  uint8_t data_for_pcf1 = upper | lower;
  pcf8574WriteOneByte(PCF8574_ADDRESS_1, data_for_pcf1); //lower digits
  
}