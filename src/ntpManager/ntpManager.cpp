#include "ntpManager.h"
#include "secrets.h"
#include "time_keeper/time_keeper.h"
#include <WiFi.h>
#include <time.h>
#include "logger/logger.h"

static bool wifiStarted = false;
static bool ntpRequested = false;
static bool needsInitialSync = true;
static bool needsDailySync = true;  
static constexpr uint32_t NTP_TRIGGER_TIME_SEC = 
    3 * 3600 + 0 * 60 + 10; //NTP trigger time is 3:00:10 AM
static int lastSyncDay = -1;
static wl_status_t lastStatus = WL_IDLE_STATUS;

static void hasDayChanged();
static void ntpUpdate();
static const char* wifiStatusToString(wl_status_t status);

bool ntpManagerInit(void) {
    WiFi.mode(WIFI_OFF);

    logInfo("NTP Manager Initialized.");
    return true;
}
void ntpRequestTimeUpdate() {

    hasDayChanged();

    // Check if it's time to request an NTP update or if its the initial sync
    // checks if time is > 3:00:10 AM AND daily sync is needed
    if(needsInitialSync || (getTime() > NTP_TRIGGER_TIME_SEC && needsDailySync)) {

    if(!wifiStarted) {
        lastStatus = WL_IDLE_STATUS;
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        wifiStarted = true;
        }
        wl_status_t status = WiFi.status();

        if(status == WL_CONNECTED && !ntpRequested) {
            ntpRequested = true;
            logInfo("WiFi connected, requesting NTP time...");
            configTzTime("ACST-9:30ACDT,M10.1.0,M4.1.0/3", "pool.ntp.org", "time.nist.gov");
        } 

    if (status != lastStatus) {
        char buffer[100];
        snprintf(buffer, sizeof(buffer), "WiFi status changed: %s", wifiStatusToString(status));
        logInfo(buffer);
        lastStatus = status;
    }
        ntpUpdate();
    }
}

static void ntpUpdate()
{
    if(!ntpRequested) {
        return; // NTP not requested yet
    }

    time_t now = time(nullptr);
    if(now > 1700000000) {
        //17b represents a unix time in 2024, so we have a valid time value

        struct tm currentTime;

        localtime_r(&now, &currentTime);
        lastSyncDay = currentTime.tm_yday;
        char timeStr[32];
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &currentTime);
        char buffer[100];
        snprintf(buffer, sizeof(buffer), "NTP time obtained: %s", timeStr);
        logInfo(buffer);
        //logInfo("NTP time obtained: " + String(asctime(&currentTime)));

        // Set the time in the timekeeper
        uint32_t secondsSinceMidnight = currentTime.tm_hour * 3600 + currentTime.tm_min * 60 + currentTime.tm_sec;
        setTimeSecondsOfTheDay(secondsSinceMidnight);

        //time set successfully, disconnect wifi to save power
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        wifiStarted = false; 
        ntpRequested = false; // Reset for next update
        needsInitialSync = false; // Initial sync done  
        needsDailySync = false;   // Daily sync done
        lastSyncDay = currentTime.tm_yday;

        logInfo("WiFi disconnected after NTP update.");
    } 
}

static void hasDayChanged() {
    time_t now = time(nullptr); // Get current time according to system
    if(now < 1700000000) {
        return; // Time not set yet
    }

    struct tm currentTime;      // create standard tm struct to hold time info
    localtime_r(&now, &currentTime); //convert current time now to struct tm format
    if(currentTime.tm_yday != lastSyncDay) {
        //check if day of year has changed since last sync
        lastSyncDay = currentTime.tm_yday;
        needsDailySync = true; // Reset daily sync flag for new day
    }
}

static const char* wifiStatusToString(wl_status_t status)
{
    switch (status) {
        case WL_IDLE_STATUS:      return "IDLE";
        case WL_NO_SSID_AVAIL:    return "NO_SSID";
        case WL_SCAN_COMPLETED:   return "SCAN_DONE";
        case WL_CONNECTED:        return "CONNECTED";
        case WL_CONNECT_FAILED:   return "CONNECT_FAILED";
        case WL_CONNECTION_LOST:  return "CONNECTION_LOST";
        case WL_DISCONNECTED:     return "DISCONNECTED";
        default:                  return "UNKNOWN";
    }
}
