#pragma once
class String;

bool loggerInit(void);

void logInfo(const char* fmt, ...);
void logGame(const char* fmt, ...);
void logWarning(const char* message);
void logError(const char* message);