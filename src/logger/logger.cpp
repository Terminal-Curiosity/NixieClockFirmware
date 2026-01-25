#include "logger.h"
#include <Arduino.h>
#include <cstdio>
#include <cstdarg>

bool loggerInit(void) {
    Serial.begin(115200);
    logInfo("Logger Initialized.");
    return true;
}

void logInfo(const char* message, ...) {

    char buffer[128];   

    va_list args;
    va_start(args, message);
    vsnprintf(buffer, sizeof(buffer), message, args);
    va_end(args);

    Serial.print("[INFO] ");
    Serial.println(buffer);
}


void logWarning(const char* message) {
    Serial.print("[WARNING] ");
    Serial.println(message);
}

void logError(const char* message) {
    Serial.print("[ERROR] ");
    Serial.println(message);
}