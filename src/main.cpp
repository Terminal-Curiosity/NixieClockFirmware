#include "setup/setup.h"
#include "time_keeper/time_keeper.h"
#include "LDR/ldr.h"
#include "nixie_tubes/nixie_tubes.h"
#include "leds/leds.h"
#include "ntpManager/ntpManager.h"


void setup();

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