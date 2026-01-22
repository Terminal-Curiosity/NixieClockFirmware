#include "wifi.h"
#include "logger/logger.h"
#include <WiFi.h>

static wl_status_t lastStatus = WL_IDLE_STATUS;
static bool wifiStarted = false;

bool wifiInit(void) {
    // Placeholder for WiFi initialization code
    WiFi.mode(WIFI_OFF);
    logInfo("WiFi Initialized.");
    return true;
}

const char* wifiStatusToString(wl_status_t status)
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

