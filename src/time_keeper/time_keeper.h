#pragma once

#include <stdint.h>

constexpr int32_t SECONDS_PER_DAY = 86400;

void timeKeeperInit();
void timebaseTick();
void setTimeSecondsOfTheDay(uint32_t seconds);
bool consumeOneSecondTick();

uint32_t getTime();