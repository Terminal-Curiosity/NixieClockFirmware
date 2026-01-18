#include "config.h"
#include "ldr/ldr.h"
#include "nixie_tubes/nixie_tubes.h"
#include "leds/leds.h"
#include "time_keeper/time_keeper.h"
#include "I2C/I2C.h"
#include "wifi/wifi.h"
#include "HVPSU/hvpsu.h"
#include "ntpManager/ntpManager.h"
#include "secrets.h" //not included in repo for security reasons

void setup() {
  Serial.begin(115200);
  Serial.println("Initialising Setup...");

  i2cInit();
  hvpsuInit();
  ldrInit();
  ledsInit();
  timeKeeperInit(); 
  wifi_init();
  ntpManagerInit();

  Serial.println("Setup Complete.");
  hvpsuEnable();
  Serial.println("HV PSU now Enabled.");
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

yield(); //yield to allow background processes to run - wifi etc.

}