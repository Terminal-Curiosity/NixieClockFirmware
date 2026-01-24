#include "setup/setup.h"
#include "time/timeKeeper.h"
#include "time/timeReporter.h"
#include "LDR/ldr.h"
#include "nixieTubes/nixieTubes.h"
#include "leds/leds.h"
#include "ntpManager/ntpManager.h"


void setup();

void loop() {
  if(consumeOneSecondTick())
  {
    nixieTubesUpdate();      //update the tube display to show the new time
    ldrReadSave();          //read LDR value and save it once per second
    
  }

  ledSlowRainbowFade();
  ntpRequestTimeUpdate();

  yield(); //yield to allow background processes to run - wifi etc.
}