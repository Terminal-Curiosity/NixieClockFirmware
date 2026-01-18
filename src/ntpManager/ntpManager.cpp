#include "ntpManager.h"
#include "secrets.h"
#include "time_keeper/time_keeper.h"
#include <Arduino.h>
#include <WiFi.h>
#include <time.h>

static bool started = false;
static bool ntpRequested = false;

void ntpManagerInit() {
    WiFi.mode(WIFI_OFF);

    Serial.println("NTP Manager Initialized."); 
}   

void ntpRequestTimeUpdate() {
    if(!started) {
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        started = true;
    }

    static uint32_t lastRequestTime = 0;
    if (millis() - lastRequestTime > 5000) { // Request update every 60 seconds
        lastRequestTime = millis();
        // Actual NTP request code would go here

        if(WiFi.status() == WL_CONNECTED && !ntpRequested) {
            ntpRequested = true;
            Serial.println("WiFi connected, requesting NTP time...");
            configTzTime("ACST-9:30ACDT,M10.1.0,M4.1.0/3", "pool.ntp.org", "time.nist.gov");
            
        } else {
            Serial.println("WiFi not connected, cannot request NTP time.");
        }

        ntpUpdate();

    }
}

void ntpUpdate()
{
    if(!ntpRequested) {
        return; // NTP not requested yet
    }

    time_t now = time(nullptr);
    if(now > 1700000000) {
        //17b represents a unix time in 2024, so we have a valid time value

        struct tm currentTime;

        localtime_r(&now, &currentTime);
        Serial.print("NTP time obtained: " + String(asctime(&currentTime)));

        // Set the time in the timekeeper
        uint32_t secondsSinceMidnight = currentTime.tm_hour * 3600 + currentTime.tm_min * 60 + currentTime.tm_sec;
        setTimeSecondsOfTheDay(secondsSinceMidnight);

        //time set successfully, disconnect wifi to save power
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        started = false; 
        ntpRequested = false; // Reset for next update
        Serial.println("WiFi disconnected after NTP update.");

    } else {
        Serial.println("Failed to obtain NTP time.");
    }
}