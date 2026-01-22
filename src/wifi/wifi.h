#pragma once 

#include <WiFi.h>

bool wifiInit(void);
wl_status_t wifiStatusReport();

bool wifiIsConnected(void);
void wifiStart(void);
bool wifiStatusHasChanged();
const char* wifiStatusToString(wl_status_t status);