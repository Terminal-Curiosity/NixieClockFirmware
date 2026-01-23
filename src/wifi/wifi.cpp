#include "wifi.h"
#include "logger/logger.h"
#include <WiFi.h>
#include "secrets.h"

static wl_status_t lastLoggedStatus = WL_IDLE_STATUS;

static int currentCred = 0;
static uint32_t attemptStartMs = 0;
static bool attemptInProgress = false;
static bool wifiEnabled = false;

// How long to spend on a single SSID before trying the next one:
static const uint32_t PER_NETWORK_MS = 8000;

// How long to wait after a full cycle through all creds before trying again:
static const uint32_t WIFI_CYCLE_RETRY_MS = 10000;

static uint32_t lastCycleEndMs = 0;

bool wifiInit(void) {
    WiFi.setAutoReconnect(false);
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    WiFi.setHostname("nixray-clock");
    logInfo("WiFi Initialized.");
    return true;
}

void wifiStart()
{
    // If already connected, return immediately
    wl_status_t status = wifiStatusReport();
    logStatusIfChanged();

    if (status == WL_CONNECTED) {
        attemptInProgress = false;
        return;
    }

    // Ensure STA mode
    if (WiFi.getMode() != WIFI_STA) {
        WiFi.mode(WIFI_STA);
    }

    // Start (or restart) connection attempt once per 10sec
    uint32_t nowMs = millis();

    //if not currently attempting to connect, start attempt
    if(!attemptInProgress)
    {
         if (currentCred >= WIFI_CREDS_COUNT) {
            if ((int32_t)(nowMs - lastCycleEndMs) < (int32_t)WIFI_CYCLE_RETRY_MS) {
                return;
            }
            currentCred = 0; // keep number of attempts within the range of actual recorded number of creds
    }   

    WiFi.disconnect(true, true);
    WiFi.begin(WIFI_CREDS[currentCred].WIFI_SSID, WIFI_CREDS[currentCred].WIFI_PASS);    

    attemptStartMs = nowMs;
    attemptInProgress = true;

    char buffer[120];
    snprintf(buffer, sizeof(buffer), "WiFi begin: trying SSID[%d]=%s",
                 currentCred, WIFI_CREDS[currentCred].WIFI_SSID);
    logInfo(buffer);

        return;
    }

    if ((int32_t)(nowMs - attemptStartMs) >= (int32_t)PER_NETWORK_MS) {
        // Timed out - move to next credential
        WiFi.disconnect(true);
        attemptInProgress = false;
        currentCred++;

        if (currentCred >= WIFI_CREDS_COUNT) {
            lastCycleEndMs = nowMs;
            logInfo("WiFi: finished cycle through credentials; entering cooldown");
        } else {
            logInfo("WiFi: timed out; moving to next credential");
        }
    }
}

void wifiStop()
{
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    logInfo("WiFi stopped");
}

bool wifiIsConnected(void)
{
    logStatusIfChanged();
    return WiFi.status() == WL_CONNECTED;
}


wl_status_t wifiStatusReport()
{
    return (WiFi.status());
}

void logStatusIfChanged() 
{
    wl_status_t status = wifiStatusReport();

    if(status == lastLoggedStatus)
    {return;}

     char buffer[100];
     snprintf(buffer, sizeof(buffer), "WiFi status changed: %s", wifiStatusToString(status));
     logInfo(buffer);
     lastLoggedStatus = status;
    
}

const char* wifiStatusToString(wl_status_t status)
{   
    if (WiFi.getMode() == WIFI_OFF) return "WIFI_OFF";
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

