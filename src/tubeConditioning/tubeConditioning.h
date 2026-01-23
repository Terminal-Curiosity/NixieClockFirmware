#pragma once

void conditioningUpdate();     // call every loop
bool conditioningIsActive();   // optional, for display logic
bool conditioningWantsDisplayTime(); // true during the 10s normal phase
int conditioningDigit(void);   // valid during sweep phase