#include "logger.h"
#include <Arduino.h>

void loggerInit() {
    Serial.begin(115200);
    logInfo("Logger Initialized.");
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