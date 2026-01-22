#pragma once

#include <stdint.h>
#include <time.h>

constexpr int32_t SECONDS_PER_DAY = 86400;

bool timeKeeperInit(void);
void timebaseTick();
void setTimeSecondsOfTheDay(time_t seconds);
bool consumeOneSecondTick();
bool dayHasChanged(void);

bool timeIsValid();
time_t reportSystemTimeUTC(void);
void reportLocalTime(struct tm& currentTime);

void startNtpTimeSync(void);
void kickNtpTimeSync(void);

uint32_t getTime();