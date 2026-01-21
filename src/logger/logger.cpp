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

void logWarning(const char* message) {
    Serial.print("[WARNING] ");
    Serial.println(message);
}
void logError(const char* message) {
    Serial.print("[ERROR] ");
    Serial.println(message);
}