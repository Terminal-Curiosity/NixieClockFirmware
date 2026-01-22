#include "timeKeeper.h"
#include <Arduino.h>
#include <esp_timer.h> 
#include "logger/logger.h"

static uint32_t secSinceMidnight = 0; // current time in seconds since midnight
static volatile bool oneSecondFlag = false;  // Flag set by the one second timer ISR
static esp_timer_handle_t one_sec_tick_timer_handle; // One second timer to increment clock time
static void IRAM_ATTR one_sec_tick_timer_callback(void* arg);

static int lastSyncDay = -1; //last day that time was updated


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

void timebaseTick() {
  secSinceMidnight++;
  if (secSinceMidnight >= 86400) { // Wrap around after 24 hours
    secSinceMidnight = 0;
  }
}

void setTimeSecondsOfTheDay(time_t seconds) {
  // secSinceMidnight = seconds;

  // uint32_t h = seconds / 3600;
  // uint32_t m = (seconds % 3600) / 60;
  // uint32_t s = seconds % 60;

  // char buffer[64];
  // snprintf(buffer, sizeof(buffer),
  //            "Time updated to %02lu:%02lu:%02lu",
  //            (unsigned long)h,
  //            (unsigned long)m,
  //            (unsigned long)s);
    
    
    // lastSyncDay = 
    // logInfo(buffer);
}

static void IRAM_ATTR one_sec_tick_timer_callback(void* arg)
{
  oneSecondFlag = true; //after one sec, set the flag true
}

uint32_t getTime() {
  noInterrupts();
  uint32_t time_copy = secSinceMidnight;
  interrupts();
  return time_copy;
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

//---------------experimental zone below- ------------------



void timeAdjustSeconds(int32_t deltaSeconds){
// Adjust the current time by a given number of seconds (can be negative). 
//  int32_t timeIntermediate = (static_cast<int32_t>(secSinceMidnight) + deltaSeconds) % SECONDS_PER_DAY;
  
//     if (timeIntermediate < 0) {
//         timeIntermediate += SECONDS_PER_DAY; // Wrap around for negative values
//     }
//     secSinceMidnight = static_cast<uint32_t>(timeIntermediate);
}
 
void startNtpTimeSync(void)
{

static bool ntpSyncStarted = false;
  if (ntpSyncStarted) return;
  configTzTime("ACST-9:30ACDT,M10.1.0,M4.1.0/3", 
    "pool.ntp.org", "time.nist.gov");
  ntpSyncStarted = true;

}

void kickNtpTimeSync(void)
//call this function to trigger a system time reset with NTP time 
{
  configTzTime("ACST-9:30ACDT,M10.1.0,M4.1.0/3", 
    "pool.ntp.org", "time.nist.gov");
}

time_t reportSystemTimeUTC() {
    return time(nullptr);
}

bool reportLocalTime(struct tm& currentTime)
{
  time_t now = reportSystemTimeUTC();
  localtime_r(&now, &currentTime);
  return true;
}

bool dayHasChanged() {
   time_t now = reportSystemTimeUTC(); // Get current time according to system
   if(!timeIsValid) {
        return false; // Time not set yet
    }

    struct tm currentTime;
    if(reportLocalTime(currentTime))
    {
        if (currentTime.tm_yday != lastSyncDay) {
        return true;
      }
    }
    return false;
    
}

bool timeIsValid(void)
{ 
  return(reportSystemTimeUTC() > 17000000000);  
}