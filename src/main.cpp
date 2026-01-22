#include "setup/setup.h"
#include "timeKeeper/timeKeeper.h"
#include "LDR/ldr.h"
#include "nixieTubes/nixieTubes.h"
#include "leds/leds.h"
#include "ntpManager/ntpManager.h"


void setup();

void loop() {
  if(consumeOneSecondTick())
  {
    //timebaseTick();         //increment the time by one second
    showCurrentTime();      //update the tube display to show the new time
    ldrReadSave();          //read LDR value and save it once per second
    ledSlowRainbowFade(10);
  }

  //ledSlowRainbowFade();
  ntpRequestTimeUpdate();
  //tubeConditioning();

  yield(); //yield to allow background processes to run - wifi etc.
}