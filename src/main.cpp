#include "setup/setup.h"
#include "time/timeKeeper.h"
#include "time/timeReporter.h"
#include "LDR/ldr.h"
#include "nixieTubes/nixieTubes.h"
#include "leds/leds.h"
#include "ntpManager/ntpManager.h"


void setup();

void loop() {

  nixieTubesUpdate();      //update the tube display
  ldrReadSave();           //read LDR value and save it
  updateLeds();            //update Leds
  ntpRequestTimeUpdate();  //run NTP update tests. update if valid

  yield(); //yield to allow background processes to run - wifi etc.
} 
