#include "ntpManager.h"
//#include "secrets.h"
#include "timeKeeper/timeKeeper.h"
//#include <WiFi.h>
#include <time.h>
#include "logger/logger.h"
#include "wifi/wifi.h"

static bool ntpRequested = false;
static bool needsInitialSync = true;
static bool needsDailySync = true;  
static bool needsSyncNow = true;

static constexpr uint32_t NTP_TRIGGER_TIME_SEC = 
    3 * 3600 + 0 * 60 + 10; //NTP trigger time is 3:00:10 AM

static int lastSyncDay = -1;
    
static void ntpUpdate(void);

void ntpRequestTimeUpdate() {

    time_t now = reportSystemTimeUTC();
    struct tm currentTime;
    reportLocalTime(currentTime);

    needsDailySync = dayHasChanged();
    if(currentTime.tm_hour * 3600
        + currentTime.tm_min *60 
        + currentTime.tm_sec >= NTP_TRIGGER_TIME_SEC)
        {
            needsSyncNow = true;
        }


    // Check if it's time to request an NTP update or if its the initial sync
    // checks if time is > 3:00:10 AM AND daily sync is needed
    if(needsInitialSync || (needsSyncNow && needsDailySync)) {
        wifiStart();

        if(wifiIsConnected() && !ntpRequested) {
            ntpRequested = true;
            logInfo("Requesting NTP time...");
            configTzTime("ACST-9:30ACDT,M10.1.0,M4.1.0/3", "pool.ntp.org", "time.nist.gov");
            bool a = wifiStatusHasChanged();
            ntpUpdate();
        }
    }
}

static void ntpUpdate(void)
{
    if(!ntpRequested) {
        return; // NTP not requested yet
    }

    time_t now = reportSystemTimeUTC();
    struct tm currentTime;
    

    if(timeIsValid()) {

        kickNtpTimeSync();

        localtime_r(&now, &currentTime);

        lastSyncDay = currentTime.tm_yday;
        char timeStr[32];
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &currentTime);
        char buffer[100];
        snprintf(buffer, sizeof(buffer), "NTP time obtained: %s", timeStr);
        logInfo(buffer);

        //time set successfully, disconnect wifi to save power
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        logInfo("WiFi disconnected after NTP update.");

        ntpRequested = false; // Reset for next update
        needsInitialSync = false; // Initial sync done  
        needsDailySync = false;   // Daily sync done
        //lastSyncDay = currentTime.tm_yday;

        
    } 
}

// static void hasDayChanged() {
//     time_t now = time(nullptr); // Get current time according to system
//     if(now < 1700000000) {
//         return; // Time not set yet
//     }

//     struct tm currentTime;      // create standard tm struct to hold time info
//     localtime_r(&now, &currentTime); //convert current time now to struct tm format
//     if(currentTime.tm_yday != lastSyncDay) {
//         //check if day of year has changed since last sync
//         lastSyncDay = currentTime.tm_yday;
//         needsDailySync = true; // Reset daily sync flag for new day
//     }
// }
