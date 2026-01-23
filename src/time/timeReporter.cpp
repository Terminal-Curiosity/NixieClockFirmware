
#include "timeReporter.h"
#include "timeKeeper.h"


int timeReporter_localDayofYear()
{
  if (!timeIsValid()) return -1;
  time_t now = timeReporter_nowUtc();
  struct tm t;
  localtime_r(&now, &t);
  return t.tm_yday;
}

uint32_t timeReporter_secondsSinceMidnight()
{
  if (!timeIsValid()) return -1;
  time_t now = timeReporter_nowUtc();
  struct tm t;
  localtime_r(&now, &t);
  return (t.tm_hour * 3600
        + t.tm_min *60 
        + t.tm_sec);
}

time_t timeReporter_nowUtc() {
    return time(nullptr);
}

void timeReporter_nowLocalTime(struct tm& currentTime)
{
  time_t now = timeReporter_nowUtc();
  localtime_r(&now, &currentTime);
}


