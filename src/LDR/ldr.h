#pragma once

#include <arduino.h>

uint8_t ldrReportRawValue();
uint8_t ldrReportValueAsPercentage();

void ldrReadSave(void);
void ldrInit(void);