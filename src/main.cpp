#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <esp_timer.h> 
#include "config.h"
#include "ldr/ldr.h"
#include "nixie_tubes/nixie_tubes.h"
// #include "nixie_tubes/nixie_tubes.cpp"
#include "clock_tick/clock_tick.h"  
#include "time_keeper/time_keeper.h"


volatile bool oneSecondFlag = false; // set by the one second timer ISR. used by loop


// --- Global Variables for Display (volatile for ISR access) ---
volatile byte current_digit_bcd_tube1 = 0x00; // Example: Display digit '0' for tube 1
volatile byte current_digit_bcd_tube2 = 0x02; // Example: Display digit '2' for tube 2
volatile byte current_digit_bcd_tube3 = 0x03; // Example: Display digit '3' for tube 3
volatile byte current_digit_bcd_tube4 = 0x08; // Example: Display digit '4' for tube 4

// --- Timer Handles ---
esp_timer_handle_t one_sec_tick_timer_handle; // One second timer to increment clock time

// --- Function Prototypes (IRAM_ATTR (instruction RAM atribute) for ISRs for reliability) ---
// IRAM_ATTR places the ISR functions in RAM instead of flash because its much faster and is not affected if flash memory is already in use elsewhere.
void IRAM_ATTR pwm_on_timer_callback(void* arg); 
void IRAM_ATTR one_sec_tick_timer_callback(void* arg);

void readLDRAndMapBrightness();
void writePcf8574(byte address, byte data);
void write1byte(byte, byte);
uint32_t ColorHSV(float h, float s, float v);
void doOneSecondTick();
void updateTubeDisplay(int value_to_display); 
void showCurrentTime();


Adafruit_NeoPixel strip = Adafruit_NeoPixel(4, led_pin, NEO_GRB + NEO_KHZ800);
bool led_update = 0;

//uint32_t currentTimeSec; // current time in seconds since midnight


void setup() {
  Wire.begin(SDA1_pin, SCL1_pin);  //initialise I2C with pins specified
  Wire.setClock(100000); // set I2C speed (default is 100kHz)
  pinMode(led_pin,OUTPUT);
  digitalWrite(led_pin, LOW);
  analogSetAttenuation(ADC_11db); // sets attenuation of LDR input (otherwise it will be 0-1V)
  pinMode(HV_enable_pin,OUTPUT); //initialise the HV pin as output and set low (HV disabled)
  digitalWrite(HV_enable_pin, LOW);
  timebaseInit(); //initialize the clock value

  strip.begin();         // Initialize NeoPixel strip
  strip.setBrightness(50);  // brightness from 0–255
  strip.show();          // Turn off all LEDs at start
  
  Serial.begin(115200);
  Serial.println("Nixie Clock Firmware Starting...");



  esp_timer_create_args_t one_sec_tick_timer_args = {
      .callback = &one_sec_tick_timer_callback, 
      .name = "one_sec_tick_timer"            
  };
  esp_timer_create(&one_sec_tick_timer_args, &one_sec_tick_timer_handle);

  // Start the main periodic timer; it will fire every PWM_PERIOD_US (e.g., 5ms)
  esp_timer_start_periodic(one_sec_tick_timer_handle, 1000000);
  Serial.println("PWM timers initialized and started.");
 
}


uint16_t hue = 0;
void loop() {

    digitalWrite(HV_enable_pin, HIGH); //enable HV supply
    //updateTubeDisplay(2026);
  if(oneSecondFlag)
  {
    timebaseTick();     //increment the time by one second
    oneSecondFlag = false; //reset the flag
    
    showCurrentTime();   //update the tube display to show the new time
  }
  // uint32_t color = ColorHSV(hue, 1, 1);

  // uint32_t led_colour_val = 0x000000FF << current_digit_bcd_tube1;
  // strip.setPixelColor(0, color);  // Set the first (only) LED
  // strip.setPixelColor(1, color);
  // strip.setPixelColor(2, color);
  // strip.setPixelColor(3, color);

  

  
  // strip.show();
  // led_update = 0;
  // hue = hue+36;
  // Serial.println(hue);
  // if(hue >= 360)
  // {
  //   hue = 0;
  // }
 }

void IRAM_ATTR one_sec_tick_timer_callback(void* arg)
{
  oneSecondFlag = true; //after one sec, set the flag true
}

