#pragma once
class String;

void loggerInit();

void log(const char* message);
void log(const String& message);

void logInfo(const char* message);
void logWarning(const char* message);
void logError(const char* message);