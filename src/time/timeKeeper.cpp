#include "timeKeeper.h"
#include <Arduino.h>
#include <esp_timer.h> 
#include "logger/logger.h"
#include "timeReporter.h"

//static uint32_t secSinceMidnight = 0; // current time in seconds since midnight
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

  //secSinceMidnight = 0; // Initialize time to midnight
  logInfo("Time Keeper Initialized.");
  return true;
}

static void IRAM_ATTR one_sec_tick_timer_callback(void* arg)
{
  oneSecondFlag = true; //after one sec, set the flag true
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

void kickNtpTimeSync(void)
//call this function to trigger a system time sync with NTP time 
{
  configTzTime("ACST-9:30ACDT,M10.1.0,M4.1.0/3", 
    "pool.ntp.org", "time.nist.gov");
}

bool timeIsValid(void)
{ 
  return(timeReporter_nowUtc() > 1704067200);  
}


