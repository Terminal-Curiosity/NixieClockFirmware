#pragma once

#include "timeKeeper.h"

uint32_t timeReporter_secondsSinceMidnight();
int timeReporter_localDayofYear();
void timeReporter_nowLocalTime(struct tm& currentTime);
time_t timeReporter_nowUtc();