#pragma once

#include <arduino.h>

uint16_t ldrReportRawValue();
uint8_t ldrReportValueAsPercentage();

void ldrReadSave(void);
bool ldrInit(void);