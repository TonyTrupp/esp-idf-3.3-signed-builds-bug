#ifndef HEADER_GLOBALS 
#define HEADER_GLOBALS

//For loading esp32 GPIO pin definitions. There might be a more direct way of doing this, but wifi is required anyway.  
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "Arduino.h"
#include "WiFi.h"
#include <string.h>
#include "WString.h"
#include "esp32-hal-gpio.h"
#include "esp_log.h"
//#include "esp32-hal-misc.c"
#include "esp_ota_ops.h"
#include "../lib/Timer/Timer.h"
#include "../lib/ArduinoJson-v6.10.0.h"
#include "Logging.h"


#define FIRMWARE_VERSION "FWV_0.0.1"

#define RELEASE_MODE false

#if RELEASE_MODE 
    #define CT_LOG_LEVEL LOG_LEVEL_WARN
#else
    #define CT_LOG_LEVEL LOG_LEVEL_DEBUG
#endif


//---------------------------------------------------------

#define ESP32 true
#define ARDUINO 100

#define PRIMARY_CORE 1
#define UI_CORE 0


//#define IS_BREADBOARD 0
#ifdef IS_BREADBOARD 

    //BREADBOARD PINS 

    #define STATUS_LED 4                // Test point used when testing PCB wiring - same on breadboard, but labeled A5 
    #define OLED_12V_POWER 12           // Only needed on PCB 
    #define OLED_CS 36                  // Chip select - GROUNDED - dummy pin on pcb, since library requires it, same on breadboard
    #define CHARGE_PIN 25               // DAC_1 / ADC2_8 / GPIO25 - same on PCB and breadboard 
    #define PROGRAM_GO_BACK_BTN 27      // same on PCB and breadboard 
    #define PROGRAM_GO_FORWARD_BTN 26   // same on PCB and breadboard

    #define OLED_RESET 14           // GPIO 2 on PCB - Note GPIO 2 must be low or unconnected at start for bootload 
    #define OLED_DC 21              // Data/Command Control - GPIO 15 on PCB 
    #define OLED_3V_POWER A3        // powers both 3v & 12v on breadboard
    #define OLED_CLK 5              // Not used explicitly, automatic with SPI, here for reference 
    #define OLED_MOSI 13 

    #define PROGRAM_ROTARY_A 32     // 35 on PCB 
    #define PROGRAM_ROTARY_B 33     // 32 on PCB 
    #define PROGRAM_ROTARY_BTN 15   // USING GPIO39/A3/SENSOR_VN on PCB (test this) 
    #define WAKEUP_PIN GPIO_NUM_13  // 33 on PCB
    #define BATTERY_MONITOR_PIN A2  //GPIO 34 on PCB  

#else  /// PCBS

    //#define IS_REV1_PCB 1
    #ifdef IS_REV1_PCB 

        //REV 1 PCB PINS 

        #define BATTERY_MONITOR_PIN 34  //ADC1_CH6  
        #define STATUS_LED 4                // Test point used when testing PCB wiring - same on breadboard, but labeled A5 
        #define OLED_12V_POWER 12           // Only needed on PCB 
        #define OLED_CS 36                  // Chip select - GROUNDED - dummy pin on pcb, since library requires it, same on breadboard
        #define CHARGE_PIN 25               // DAC_1 / ADC2_8 / GPIO25 - same on PCB and breadboard 

        #define OLED_RESET 2           // Note GPIO 2 must be low or unconnected at start for bootload 
        #define OLED_DC 15             //Data/Command Control 
        #define OLED_3V_POWER 5        
        #define OLED_CLK 14             
        #define OLED_MOSI 13 

        #define PROGRAM_ROTARY_A 32 //DONT USE 35 on pcb, NO PULLUP!      
        #define PROGRAM_ROTARY_B 35  
        #define PROGRAM_GO_BACK_BTN 27      
        #define PROGRAM_GO_FORWARD_BTN 26   
        #define PROGRAM_ROTARY_BTN 39   //USING GPIO39/A3/SENSOR_VN on PCB (test this) 
        #define WAKEUP_PIN GPIO_NUM_33 

    #else 

        //REV 2 PCB PINS 
        //Listed in counter clockwise order, left to right, around esp32 module 

        #define BATTERY_MONITOR_PIN 34      // ADC1_CH6 
        #define CHARGE_PIN 32               // DAC_1 / ADC2_8 / GPIO25 - same on PCB and breadboard 

        #define OLED_DC 33                  // Data/Command Control 
        #define OLED_RESET 25               // Note GPIO 2 must be low or unconnected at start for bootload 
        #define OLED_3V_POWER 27            // Supplies logic level power to OLED
        #define OLED_CLK 14                 // SPI Clock
        #define OLED_12V_POWER 12           // Turns on the 12v Boost Converter 
        #define OLED_MOSI 13                // SPI Master Out / Slave In data 
        #define OLED_CS 36                  // DUMMY: Chip select - GROUNDED - fake pin on pcb, since library requires it, same on breadboard 

        #define WAKEUP_PIN GPIO_NUM_15      // Wake-up inputs from rotary, left and right button presses 
        #define STATUS_LED 4                // Test point used when testing PCB wiring 
        #define PROGRAM_ROTARY_A 16         // Knob Rotary Encoder Gate A
        #define PROGRAM_ROTARY_B 17         // Knob Rotary Encoder Gate B
        #define PROGRAM_ROTARY_BTN 5        // Knob Button Press 
        #define PROGRAM_GO_BACK_BTN 19      // Left Button 
        #define PROGRAM_GO_FORWARD_BTN 18   // Right Button 

    #endif

#endif //End Breadboard or PCB 


#endif