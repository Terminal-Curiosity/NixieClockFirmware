#include "logger.h"
#include <Arduino.h>

bool loggerInit(void) {
    Serial.begin(115200);
    logInfo("Logger Initialized.");
    return true;
}

void logInfo(const char* message) {
    Serial.print("[INFO] ");
    Serial.println(message);
}

void logInfo(const char* msg1, const char* msg2)
{
    Serial.print("[INFO] ");
    Serial.print(msg1);
    Serial.println(msg2);
}

void logWarning(const char* message) {
    Serial.print("[WARNING] ");
    Serial.println(message);
}

void logError(const char* message) {
    Serial.print("[ERROR] ");
    Serial.println(message);
}