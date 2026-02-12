#include "tubeConditioning.h"
#include "time/timeReporter.h"

static constexpr int32_t CONDITION_START_TIME_SEC = 
    3 * 3600 + 30 * 60 + 00; //NTP trigger time is 3:30:00 AM
static constexpr int32_t CONDITION_END_TIME_SEC = 
    3 * 3600 + 45 * 60 + 00; //NTP trigger time is 3:45:00 AM

bool conditioningIsActive()   // check if its time to activate conditioning
{
    int32_t sec = timeReporter_secondsSinceMidnight();
    if (sec < 0) return false;

    if (CONDITION_START_TIME_SEC < CONDITION_END_TIME_SEC){
        return (sec >= CONDITION_START_TIME_SEC) && (sec < CONDITION_END_TIME_SEC);
    }
    else
    {
        return (sec >= CONDITION_START_TIME_SEC) || (sec < CONDITION_END_TIME_SEC); 
        //just in case the conditioning window crosses over midnight for some reason
    }
}

ConditioningPhase conditioningPhase() //return which phase of the conditioning cycle should be active
{
    int32_t sec = timeReporter_secondsSinceMidnight();
    if (sec < 0) return SHOW_TIME;

    // Outside conditioning window. irrelevant, but safe default
    if (sec < CONDITION_START_TIME_SEC || sec >= CONDITION_END_TIME_SEC) {
        return SHOW_TIME;
    }

    // 20-second cycle:
    //  0–9 : SHOW_TIME
    // 10–19: SWEEP_DIGITS
    int32_t phasePos = (sec - CONDITION_START_TIME_SEC) % 20;
    return (phasePos < 10) ? SHOW_TIME : SWEEP_DIGITS;
} 
