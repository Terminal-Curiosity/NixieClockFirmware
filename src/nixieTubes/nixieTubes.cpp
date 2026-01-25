#include "nixieTubes.h"
#include "pcf8574/pcf8574.h"
#include "time/timeKeeper.h"
#include "time/timeReporter.h"
#include "tubeConditioning/tubeConditioning.h"
#include "logger/logger.h"

static constexpr uint8_t digit_to_pin_mapping[] = {9,0,8,4,12,2,10,6,14,1}; 
//lookup table to map digit values (0-9) to corresponding nixie digits as per wiring between K155ID1 and PCF8574

struct tm currentTime;
static DisplayMode resolveDisplayMode();
static void renderConditioning();

static DisplayMode previousDisplayMode = MODE_UNKNOWN;

const char* displayModeToString(DisplayMode);

void nixieTubesUpdate()
{

    //alarmUpdate();
    //timeSetUiUpdate();

    
    DisplayMode currentDisplayMode = resolveDisplayMode();

     switch (currentDisplayMode) {
    case MODE_ALARM_RINGING:
      //renderAlarmRinging();
      break;

    case MODE_SET_TIME:
      //renderTimeSetUi();              // show the candidate time being edited
      break;

    case MODE_ALARM_SET:
      //renderAlarmSetUi();
      break;

    case MODE_CONDITIONING:
      renderConditioning();           // time 10s / sweep digit
      break;

    case MODE_NORMAL_TIME:
    default:
      renderNormalTime();
      break;
  }

  if(currentDisplayMode != previousDisplayMode)
  {
     logInfo("Display mode changed: %s", displayModeToString(currentDisplayMode));
     previousDisplayMode = currentDisplayMode;
  }

}

const char* displayModeToString(DisplayMode currentDisplayMode)
{
    switch (currentDisplayMode) {
        case MODE_NORMAL_TIME:   return "NORMAL";
        case MODE_CONDITIONING:  return "CONDITIONING";
        case MODE_SET_TIME:      return "SET_TIME";
        case MODE_ALARM_RINGING: return "ALARM RINGING";
        case MODE_ALARM_SET:     return "SET_ALARM";
        default:                 return "UNKNOWN";
    }
}

void renderConditioning()
{
    switch (conditioningPhase()) {
        case SHOW_TIME:
            renderNormalTime();
            break;

        case SWEEP_DIGITS:
            int32_t sec = timeReporter_secondsSinceMidnight();
            uint8_t digit = (uint8_t)(sec % 10);
            tubesDisplayValue(digit * 1111);
            break;
    }
}

void renderNormalTime()
{
  if(!timeIsValid())
  {
    tubesDisplayValue(6666);
    return;
  }
  
  timeReporter_nowLocalTime(currentTime);
  uint8_t hours = currentTime.tm_hour;
  uint8_t minutes = currentTime.tm_min;
  uint8_t seconds = currentTime.tm_sec;

  //int16_t displayTime = hours * 100 + minutes; //display HHMM only
  int16_t displayTime = minutes * 100 + seconds; //display MMSS only

  tubesDisplayValue(displayTime);
  }

static DisplayMode resolveDisplayMode()
{
  // Highest priority first:
  //if (alarmIsRinging()) return MODE_ALARM_RINGING;
  
  //if (timeSetUiIsActive()) return MODE_SET_TIME;

  //if (alarmSetUiIsActive()) return MODE_ALARM_SET;

  if (conditioningIsActive()) {
   return MODE_CONDITIONING;
  }
    

  return MODE_NORMAL_TIME;
}
 

void tubesDisplayValue(uint16_t value_to_display)
//update the tube display to show the value passed into the function.
//NOTE: tubes 1 and 3 have non-standard digit wiring, so the upper nibbles must be remapped before being used.
{
  if(value_to_display > 9999)
  {
  value_to_display %= 10000;
  }
  if(value_to_display <0)
  {
    value_to_display = 0;
  }

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