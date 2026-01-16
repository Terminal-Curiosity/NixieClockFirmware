

#include "config.h"
#include "ldr/ldr.h"
#include "nixie_tubes/nixie_tubes.h"
#include "leds/leds.h"
// #include "nixie_tubes/nixie_tubes.cpp"
#include "time_keeper/time_keeper.h"
#include "I2C/I2C.h"
#include "wifi/wifi.h"

// --- Global Variables for Display (volatile for ISR access) ---
volatile byte current_digit_bcd_tube1 = 0x00; // Example: Display digit '0' for tube 1
volatile byte current_digit_bcd_tube2 = 0x02; // Example: Display digit '2' for tube 2
volatile byte current_digit_bcd_tube3 = 0x03; // Example: Display digit '3' for tube 3
volatile byte current_digit_bcd_tube4 = 0x08; // Example: Display digit '4' for tube 4

Adafruit_NeoPixel strip = Adafruit_NeoPixel(4, ledStringDin, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  Serial.println("Initialising Setup...");

  i2c_Init();

  pinMode(ledStringDin,OUTPUT);
  digitalWrite(ledStringDin, LOW);
  analogSetAttenuation(ADC_11db); // sets attenuation of LDR input (otherwise it will be 0-1V)
  pinMode(HV_enable_pin,OUTPUT); //initialise the HV pin as output and set low (HV disabled)
  digitalWrite(HV_enable_pin, LOW);
  timeKeeperInit(); //initialize the clock value

  strip.begin();         // Initialize NeoPixel strip
  strip.setBrightness(254);  // brightness
  strip.show();          // Turn off all LEDs at start
  wifi_init();
  Serial.println("Setup Complete.");
  
}


uint16_t hue = 0;
void loop() {

    digitalWrite(HV_enable_pin, HIGH); //enable HV supply
    //updateTubeDisplay(2026);

   uint32_t color = colorHSV(hue, 1.0, 1.0); //hue from 0-360, saturation 0-1, value 0-1uint32_t color = ColorHSV(hue, 1, 1);


  if(consumeOneSecondTick())
  {
    timebaseTick();         //increment the time by one second
    showCurrentTime();      //update the tube display to show the new time

    

  }
    strip.setPixelColor(0, color);  // Set the first LED
    strip.setPixelColor(1, color);
    strip.setPixelColor(2, color);
    strip.setPixelColor(3, color);
    strip.show();
    hue = hue+3;
     Serial.println(hue);
  if(hue >= 360)
  {
    hue = 0;
  }
  delay(100);
 }



