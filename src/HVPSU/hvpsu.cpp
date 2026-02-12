#include "hvpsu.h"
#include <Arduino.h>
#include "logger/logger.h"

static bool hv_is_enabled = false; // Track HV PSU state

bool hvpsuInit(void)
{
    pinMode(HV_enable_pin,OUTPUT); //initialise the HV pin as output 
    hvpsuDisable(); //set HV PSU to disabled state at startup
    logInfo("HV PSU Initialized.");
    return true;
}

void hvpsuEnable()
{
    digitalWrite(HV_enable_pin, HIGH); //enable HV supply
    hv_is_enabled = true;
    logInfo("HV PSU Enabled.");
}

void hvpsuDisable()
{
    digitalWrite(HV_enable_pin, LOW); //disable HV supply
    hv_is_enabled = false;
    logInfo("HV PSU Disabled.");
}

bool hvpsuIsEnabled()
//check recent instruction if HV PSU is enabled
{
    return hv_is_enabled;
}