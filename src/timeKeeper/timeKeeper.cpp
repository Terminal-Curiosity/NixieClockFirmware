#include "timeKeeper.h"
#include <Arduino.h>
#include <esp_timer.h> 
#include "logger/logger.h"

static uint32_t currentTimeSec; // current time in seconds since midnight
static volatile bool oneSecondFlag = false;  // Flag set by the one second timer ISR
static esp_timer_handle_t one_sec_tick_timer_handle; // One second timer to increment clock time
static void IRAM_ATTR one_sec_tick_timer_callback(void* arg);

bool timeKeeperInit(void) {
  esp_timer_create_args_t one_sec_tick_timer_args = {
      .callback = &one_sec_tick_timer_callback, 
      .name = "one_sec_tick_timer"           
  };

  esp_timer_create(&one_sec_tick_timer_args, &one_sec_tick_timer_handle);
  esp_timer_start_periodic(one_sec_tick_timer_handle, 1000000);

  currentTimeSec = 0; // Initialize time to midnight
  logInfo("Time Keeper Initialized.");
  return true;
}

void timebaseTick() {
  currentTimeSec++;
  if (currentTimeSec >= 86400) { // Wrap around after 24 hours
    currentTimeSec = 0;
  }
}

void setTimeSecondsOfTheDay(uint32_t seconds) {
  currentTimeSec = seconds;

  uint32_t h = seconds / 3600;
  uint32_t m = (seconds % 3600) / 60;
  uint32_t s = seconds % 60;

  char buffer[64];
  snprintf(buffer, sizeof(buffer),
             "Time updated to %02lu:%02lu:%02lu",
             (unsigned long)h,
             (unsigned long)m,
             (unsigned long)s);

    logInfo(buffer);
}

uint32_t getTime() {
  noInterrupts();
  uint32_t time_copy = currentTimeSec;
  interrupts();
  return time_copy;
}

void timeAdjustSeconds(int32_t deltaSeconds){
// Adjust the current time by a given number of seconds (can be negative). 
 int32_t timeIntermediate = (static_cast<int32_t>(currentTimeSec) + deltaSeconds) % SECONDS_PER_DAY;
  
    if (timeIntermediate < 0) {
        timeIntermediate += SECONDS_PER_DAY; // Wrap around for negative values
    }
    currentTimeSec = static_cast<uint32_t>(timeIntermediate);
}



bool consumeOneSecondTick()
{
  if(!oneSecondFlag)
  {
    return false;
  }
    oneSecondFlag = false;
    return true;
}
   
static void IRAM_ATTR one_sec_tick_timer_callback(void* arg)
{
  oneSecondFlag = true; //after one sec, set the flag true
}