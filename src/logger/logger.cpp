#include "logger.h"
#include <Arduino.h>
#include <cstdio>
#include <cstdarg>
#include "ble/bleLogger.h"


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
    bleLoggerWrite(buffer); 
}

void logGame(const char* message, ...) {

    char buffer[128];   

    va_list args;
    va_start(args, message);
    vsnprintf(buffer, sizeof(buffer), message, args);
    va_end(args);

    Serial.print("[GAME] ");
    Serial.println(buffer);
    
}



void logWarning(const char* message, ...) {
    char buffer[128];   

    va_list args;
    va_start(args, message);
    vsnprintf(buffer, sizeof(buffer), message, args);
    va_end(args);

    Serial.print("[WARNING] ");
    Serial.println(buffer);
    bleLoggerWrite(buffer); 
}

void logError(const char* message, ...) {
    char buffer[128];   

    va_list args;
    va_start(args, message);
    vsnprintf(buffer, sizeof(buffer), message, args);
    va_end(args);

    Serial.print("[ERROR] ");
    Serial.println(buffer);
    bleLoggerWrite(buffer); 

}