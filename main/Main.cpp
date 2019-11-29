#include "src/Globals.h" 
#include "src/HttpRequestManager.h" 
#include "src/WifiManager.h" 
#include "src/FirmwareUpdates.h"
#include "src/Logging.h"


Timer timer;  
  
TaskHandle_t primaryLoopTaskHandle = NULL;
static const char* TAG = "Main";

const short SubRoutineHierachyLength = 7;
bool (*SubRoutineHierachy[SubRoutineHierachyLength]) ();


void setup(){
    
    Serial.begin(115200);
    while (!Serial);

    unsigned long startTime = millis();

    CT_LOG_LINE();
    CT_LOGI(TAG, "Start up"); 
    

    //Statistics 
    CT_LOGI(TAG, "FIRMWARE VERSION: %s", FIRMWARE_VERSION);
    CT_LOGI(TAG, "ESP-IDF Version: %s", esp_get_idf_version());
    CT_LOGI(TAG, "Device Info: %s", GetDeviceInfo());
    
    StartFirmwareDownload();
    
    CT_LOGW(TAG, "Startup Speed: %ld milliseconds", (millis() - startTime) ); 
    CT_LOG_LINE();
}





//REPLACING STANDARD ARDUINO WIRING, WITH MORE MAIN TASK MEMORY TO PREVENT STACK OVERFLOW 
void PrimaryLoopTask(void *pvParameters){

    setup();

    ESP_LOGI(TAG, "UILoopTask Started ");

    for(;;) {
        esp_task_wdt_reset(); 

        timer.update(); 

        vTaskDelay(1);
    }
}

extern "C" void app_main(){
    //loopTaskWDTEnabled = false;
    initArduino();  
    xTaskCreateUniversal(PrimaryLoopTask, "PrimaryLoopTask", 1024 * 16, NULL, 2, &primaryLoopTaskHandle, PRIMARY_CORE);
}



