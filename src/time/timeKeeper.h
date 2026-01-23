#pragma once

#include <stdint.h>
#include <time.h>

constexpr int32_t SECONDS_PER_DAY = 86400;

bool timeKeeperInit(void);
bool consumeOneSecondTick();
time_t reportSystemTimeUTC(void);

void kickNtpTimeSync(void);
bool timeIsValid();
