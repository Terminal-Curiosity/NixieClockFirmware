#pragma once

#error "Do not include secrets.example.h. Copy it to secrets.h and include secrets.h instead."


// Template file — copy to secrets.h and fill in real credentials.

// Each credential entry has two named fields.
typedef struct {
    const char* WIFI_SSID;
    const char* WIFI_PASS;
} wifi_cred_t;

// Replace these with your real WiFi details in secrets.h
static const wifi_cred_t WIFI_CREDS[] = {
    { "YOUR_WIFI_SSID", "YOUR_WIFI_PASSWORD" },
    //{ "ANOTHER_SSID", "ANOTHER_PASSWORD" },

    // Add more credentials as needed
};

// How many credentials are in the list
static const size_t WIFI_CREDS_COUNT = sizeof(WIFI_CREDS) / sizeof(WIFI_CREDS[0]);
