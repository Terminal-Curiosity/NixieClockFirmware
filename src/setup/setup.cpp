#include "config.h"
#include "ldr/ldr.h"
#include "nixieTubes/nixieTubes.h"
#include "leds/leds.h"
#include "timeKeeper/timeKeeper.h"
#include "I2C/I2C.h"
#include "wifi/wifi.h"
#include "HVPSU/hvpsu.h"
#include "ntpManager/ntpManager.h"
#include "logger/logger.h"
#include "secrets.h" //not included in repo for security reasons
  
static bool loggerOk = false;
static bool i2cOk = false;
static bool hvpsuOk = false;   
static bool ldrOk = false;
static bool ledsOk = false;
static bool wifiOk = false;
static bool ntpOk = false;
static bool timeKeeperOk = false;

void setup() 
{
  loggerOk = loggerInit();
  i2cOk = i2cInit();
  hvpsuOk = hvpsuInit();
  ldrOk = ldrInit();
  ledsOk = ledsInit();
  wifiOk = wifi_init();
  ntpOk = ntpManagerInit();
  timeKeeperOk = timeKeeperInit();
  
  hvpsuEnable();
  logInfo("------------ Setup Complete ----------------");
}