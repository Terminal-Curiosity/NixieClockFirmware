#include "config.h"
#include "ldr/ldr.h"
#include "nixie_tubes/nixie_tubes.h"
#include "leds/leds.h"
#include "time_keeper/time_keeper.h"
#include "I2C/I2C.h"
#include "wifi/wifi.h"
#include "HVPSU/hvpsu.h"
#include "ntpManager/ntpManager.h"
#include "logger/logger.h"
#include "secrets.h" //not included in repo for security reasons


void setup() {
  loggerInit();
  i2cInit();
  hvpsuInit();
  ldrInit();
  ledsInit();
  wifi_init();
  ntpManagerInit();
  timeKeeperInit();
  logInfo("Setup Complete.");
  hvpsuEnable();
}


void loop() {
  if(consumeOneSecondTick())
  {
    timebaseTick();         //increment the time by one second
    showCurrentTime();      //update the tube display to show the new time
    ldrReadSave();          //read LDR value and save it once per second
  }

 ledSlowRainbowFade();
 ntpRequestTimeUpdate();
 //tubeConditioningCheck();

yield(); //yield to allow background processes to run - wifi etc.

}