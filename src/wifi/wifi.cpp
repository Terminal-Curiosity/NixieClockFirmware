#include "wifi.h"
#include "logger/logger.h"
#include <WiFi.h>
#include "secrets.h"

static wl_status_t lastStatus = WL_IDLE_STATUS;
static bool wifiStarted = false;

bool wifiInit(void) {
    // Placeholder for WiFi initialization code
    WiFi.mode(WIFI_OFF);
    logInfo("WiFi Initialized.");
    return true;
}

void wifiStart()
{
    // If already connected or already trying, don't restart
    wl_status_t st = WiFi.status();
    if (st == WL_CONNECTED || st == WL_IDLE_STATUS) {
        return;
    }

    // Ensure STA mode
    if (WiFi.getMode() != WIFI_STA) {
        WiFi.mode(WIFI_STA);
    }

    // Start (or restart) connection attempt
    WiFi.begin(WIFI_SSID, WIFI_PASS);
}


bool wifiIsConnected(void)
{
    return WiFi.status() == WL_CONNECTED;
}


wl_status_t wifiStatusReport()
{
    return (WiFi.status());
}

bool wifiStatusHasChanged()
{
    wl_status_t status = wifiStatusReport();

    char buffer[100];
    snprintf(buffer, sizeof(buffer), "WiFi status changed: %s", wifiStatusToString(status));
    logInfo(buffer);
    lastStatus = status;
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

