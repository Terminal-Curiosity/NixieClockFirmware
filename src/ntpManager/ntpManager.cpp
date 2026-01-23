#include "ntpManager.h"
#include "time/timeKeeper.h"
#include "time/timeReporter.h"
#include <time.h>
#include "logger/logger.h"
#include "wifi/wifi.h"

static bool ntpRequested = false;
static bool needsInitialSync = true;  
static bool needsSyncNow = true;

static int lastNtpSyncDay = -1; //last day that time was updated

static constexpr uint32_t NTP_TRIGGER_TIME_SEC = 
    3 * 3600 + 0 * 60 + 10; //NTP trigger time is 3:00:10 AM
    
static void ntpUpdate(void);

void ntpRequestTimeUpdate() {

    if(ntpRequested)
    {
        ntpUpdate();
        return;
    }

    int today = timeReporter_localDayofYear();
    bool syncedToday = timeIsValid() && today > 0 && (lastNtpSyncDay == today); //needs a daily sync if time is invalid or the day has changed

    struct tm currentTime;
    timeReporter_nowLocalTime(currentTime);
    

    uint32_t secSinceMidnight = timeReporter_secondsSinceMidnight();
    
        if(timeIsValid()){
            needsSyncNow = secSinceMidnight >= NTP_TRIGGER_TIME_SEC;
        }

    
    // Check if it's time to request an NTP update or initial sync
    // checks if time is > 3:00:10 AM AND daily sync hasnt happened
    if(needsInitialSync || (needsSyncNow && !syncedToday)) {
        wifiStart();
        if(wifiIsConnected() && !ntpRequested) {
            ntpRequested = true;
            logInfo("Requesting NTP time...");
            kickNtpTimeSync();
        }
    }

    if(lastNtpSyncDay != today)
    {
        syncedToday = false;
    }
}

static void ntpUpdate(void)
{
    struct tm currentTime;
    
    if(timeIsValid()) {
        timeReporter_nowLocalTime(currentTime);
        lastNtpSyncDay = timeReporter_localDayofYear();
        char timeStr[32];
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &currentTime);
        char buffer[100];
        snprintf(buffer, sizeof(buffer), "NTP time obtained: %s", timeStr);
        logInfo(buffer);

        //time set successfully, disconnect wifi to save power
        wifiStop();

        ntpRequested = false; // Reset for next update
        needsInitialSync = false; // Initial sync done  
    } 
}
