#include "hvpsu.h"
#include <Arduino.h>

static bool hv_is_enabled = false; // Track HV PSU state

void hvpsuInit()
{
    pinMode(HV_enable_pin,OUTPUT); //initialise the HV pin as output 
    hvpsuDisable(); //set HV PSU to disabled state at startup
}

void hvpsuEnable()
{
    digitalWrite(HV_enable_pin, HIGH); //enable HV supply
    hv_is_enabled = true;
}

void hvpsuDisable()
{
    digitalWrite(HV_enable_pin, LOW); //disable HV supply
    hv_is_enabled = false;
}

bool hvpsuIsEnabled()
//check recent instruction if HV PSU is enabled
{
    return hv_is_enabled;
}