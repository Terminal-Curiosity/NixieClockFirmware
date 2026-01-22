#pragma once 

#include <WiFi.h>

bool wifiInit(void);

const char* wifiStatusToString(wl_status_t status);