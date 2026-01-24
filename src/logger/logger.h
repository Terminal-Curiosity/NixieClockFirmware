#pragma once
class String;

bool loggerInit(void);

void logInfo(const char* message);
void logInfo(const char* msg1, const char* msg2);
void logWarning(const char* message);
void logError(const char* message);