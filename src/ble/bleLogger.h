#pragma once
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool bleLoggerInit(const char* deviceName);

// Safe to call from anywhere: just queues.
void bleLoggerWrite(const char* msg);

// Call often from loop() to actually transmit queued logs.
void bleLoggerPoll(void);

#ifdef __cplusplus
}
#endif
