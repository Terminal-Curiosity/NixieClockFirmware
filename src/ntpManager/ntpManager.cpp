#include "ntpManager.h"
#include "secrets.h"
#include <Arduino.h>
#include <WiFi.h>

static bool started = false;

void ntpManagerInit() {
    // Placeholder for NTP manager initialization code
    WiFi.mode(WIFI_OFF);
}   

void ntpRequestTimeUpdate() {
    // Placeholder for requesting an NTP time update
    if(!started) {
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        started = true;
    }

    static uint32_t lastRequestTime = 0;
    if (millis() - lastRequestTime > 5000) { // Request update every 60 seconds
        lastRequestTime = millis();
        Serial.println("WiFi status: ");
        Serial.println((int)WiFi.status());
        // Actual NTP request code would go here
    }
}