#pragma once
class String;

bool loggerInit(void);

void logInfo(const char* message);
void logWarning(const char* message);
void logError(const char* message);