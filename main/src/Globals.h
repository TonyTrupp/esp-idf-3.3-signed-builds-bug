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



#endif