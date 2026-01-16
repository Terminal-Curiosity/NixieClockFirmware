#include "time_keeper.h"
#include <Arduino.h>
#include <esp_timer.h> 

static uint32_t currentTimeSec; // current time in seconds since midnight
static bool oneSecondFlag = false;  // Flag set by the one second timer ISR
esp_timer_handle_t one_sec_tick_timer_handle; // One second timer to increment clock time
void IRAM_ATTR one_sec_tick_timer_callback(void* arg);

void timebaseInit() {
  esp_timer_create_args_t one_sec_tick_timer_args = {
      .callback = &one_sec_tick_timer_callback, 
      .name = "one_sec_tick_timer"            
  };

  esp_timer_create(&one_sec_tick_timer_args, &one_sec_tick_timer_handle);
  esp_timer_start_periodic(one_sec_tick_timer_handle, 1000000);
  Serial.println("PWM timers initialized and started.");

  currentTimeSec = 0; // Initialize time to midnight
}

void timebaseTick() {
  currentTimeSec++;
  if (currentTimeSec >= 86400) { // Wrap around after 24 hours
    currentTimeSec = 0;
  }
}

void setTimeSecondsOfTheDay(uint32_t seconds) {
  currentTimeSec = seconds % SECONDS_PER_DAY;
}

uint32_t getTime() {
  return currentTimeSec;
}

void timeAdjustSeconds(int32_t deltaSeconds){
// Adjust the current time by a given number of seconds (can be negative). 
 int32_t timeIntermediate = (static_cast<int32_t>(currentTimeSec) + deltaSeconds) % SECONDS_PER_DAY;
  
    if (timeIntermediate < 0) {
        timeIntermediate += SECONDS_PER_DAY; // Wrap around for negative values
    }
    currentTimeSec = static_cast<uint32_t>(timeIntermediate);
}

bool consumeTickFlag()
{
  if(!oneSecondFlag)
  {
    return false;
  }
    oneSecondFlag = false;
    return true;
}
   


void IRAM_ATTR one_sec_tick_timer_callback(void* arg)
{
  oneSecondFlag = true; //after one sec, set the flag true
}