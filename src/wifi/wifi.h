#pragma once 

#include <WiFi.h>

bool wifiInit(void);
wl_status_t wifiStatusReport();

bool wifiIsConnected(void);
void wifiStart(void);
void wifiStop(void);
void logStatusIfChanged();
const char* wifiStatusToString(wl_status_t status);