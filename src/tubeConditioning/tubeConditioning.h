#pragma once

#include <stdint.h>

enum ConditioningPhase {
    SHOW_TIME,
    SWEEP_DIGITS
};

bool conditioningIsActive();   
ConditioningPhase conditioningPhase(void); 
