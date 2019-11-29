#include "Globals.h"
#include "FirmwareUpdates.h"
//#include "MenuSystem/MenuProgramDisplay.h"
//#include "MenuSystem/MenuSystem.h"
//#include "MenuSystem/MenuTree.h"
//#include "OLEDDisplay.h"
#include "WifiManager.h"
//#include "PersistantStorage.h"
//#include "Sleep.h"
//#include "Provisioning.h"
//#include "InactivityWatcher.h"
#include "HttpRequestManager.h"
#include <Update.h>
#include "Logging.h"

//required for changing the ESP_IMAGE_HEADER_MAGIC 
//#include <esp_image_format.h> 

//#include "esp_event.h" 

//#include "esp_ota_ops.h"
//#include "esp_http_client.h"
//#include "esp_https_ota.h"
//#include "esp_tls.h"


#define OTA_BUF_SIZE 256
int percComplete = 0; 
static const char *TAG = "FirmwareUpdates";
char* highestVersionOnAPI;
bool updatePrompted = 0;

// TODO: set this back to 1 day, currently set to one minute  
long timeBeforePromptFirmwareAgain = 60 * 60 * 24;// / (24 * 60 * 60);  

//0=unknown, 1=update available, -1=up to date 
short cachedFirmwareCheckResult = 0;

/*
bool PromptFirmwareUpdate(){

    if(!WifiManager.WifiConnected() || !FirmwareUpdateAvailable() || updatePrompted || IsSleepWakeupReset()) 
        return false;

    //Don't show this new firmware message too frequently 
    long lastPromptTime = GetLastFirmwarePromptTime();
    if( lastPromptTime!=0 && 
        lastPromptTime + timeBeforePromptFirmwareAgain > GetCurrentUpdateUnixTimestamp() && 
        GetCurrentUpdateUnixTimestamp()!=0)
    {
            updatePrompted = 1;
            return false;
    }
    
    //Save time this was shown 
    SetLastFirmwarePromptTime(GetCurrentUpdateUnixTimestamp());
    PersistantDataCommit();

    //RestartInactivityWatcher();

    //ShowLoading(false);
    //PrintToDisplay(F("Firmware Version"), F("Update Available") ); 
    SetHoldCurrentDisplay(true, 3, true);
    
    TriggerMenuItem* firmwareMenuItem = GetFirmwareVersionMenuItem();
    firmwareMenuItem->parent = CurrentMenuItem();
    SetCurrentMenuItem(*firmwareMenuItem);
    SetUpdateDisplayPending(true);

    updatePrompted = 1;

    return true;
}
*/


bool FirmwareUpdateAvailable(){
    if(highestVersionOnAPI == NULL || highestVersionOnAPI[0] == '\0') 
        return false;

    if(cachedFirmwareCheckResult == 1) return true;
    if(cachedFirmwareCheckResult == -1) return false;

    CT_LOGI(TAG, "FirmwareUpdateAvailable cachedFirmwareCheckResult == 0");
    
    char currentVersionTmp[16];
    strncpy( currentVersionTmp, FIRMWARE_VERSION + 4, sizeof(FIRMWARE_VERSION) - 4 );

    if (strcmp(currentVersionTmp,highestVersionOnAPI)==0){ 
        cachedFirmwareCheckResult = -1;
        return false;
    }

    char highestVersionTmp[16];
    strncpy( highestVersionTmp, highestVersionOnAPI, 16 );

    int highestVersionParts[3];
    char* segment = strtok(highestVersionTmp, ".");
    short i=0;
    while (i<3){
        highestVersionParts[i] = segment != 0 ? atoi(segment) : 0;
        if(segment != 0) segment = strtok(NULL, ".");
        i++;
    }

    i=0;
    int currentVersionParts[3];
    segment = strtok(currentVersionTmp, ".");
    while (i<3){
        currentVersionParts[i] = segment != 0 ? atoi(segment) : 0;
        if(segment != 0) segment = strtok(NULL, ".");
        i++;
    }
    
    for (i=0;i<3;i++)
    {
        if (currentVersionParts[i] < highestVersionParts[i]){
            cachedFirmwareCheckResult = 1;
            CT_LOGI(TAG, "Newer Firmware Version Available on API");
            return true;
        }

        if (currentVersionParts[i] > highestVersionParts[i]){
            cachedFirmwareCheckResult = -1;
            CT_LOGI(TAG, "Only Older Firware Version Available on API");
            return false;
        }

        //do nothing in this loop if the numbers are the same 
    }

    cachedFirmwareCheckResult = -1;
    return false;
}


void SetHighestFirmwareAPIVersion(char* highestVersion){
    highestVersionOnAPI = highestVersion;
}

/*
void FirmwareUpdateExecute(MenuItem* menuItem){ 
    
    CT_LOGI(TAG, "FirmwareUpdateExecute: %d", cachedFirmwareCheckResult);

    if( cachedFirmwareCheckResult == -1 ){
        CT_LOGI(TAG, "FirmwareUpdateExecute !FirmwareUpdateAvailable");
        //ShowLoading(false);
        menuItem->isExecuting = false;
        SetCurrentMenuItem(*(menuItem->parent), false, menuItem->positionUnderParent+1 );

    }else if(!WifiManager.WifiConnected()){
        menuItem->isExecuting = false;
        //ShowLoading(false);
        //PrintToDisplay(F("Connect to WiFi"), F("before updating")); 
	    //SetUpdateDisplayPending(true);
        //SetHoldCurrentDisplay(true,3,true);
        SetCurrentMenuItem(*(menuItem->parent), false, menuItem->positionUnderParent+1);

    } else { 
        //Make call to update version 
        CT_LOGI(TAG, "FirmwareUpdateExecute Fire!");
        StartFirmwareDownload();
    }
}
*/


void DisplayPercentDownloaded(){
    //ShowLoading(true);
    char percentDownloadedText[20]; 
    sprintf(percentDownloadedText, "%d%% downloaded", percComplete);
    //PrintToDisplay(F("Firmware Updating"), percentDownloadedText);
}

/*
bool FirmwareUpdateDisplay(MenuItem* menuItem){

    FirmwareUpdateAvailable();

    char firmwareVersionStr[22]; 
    
    if (menuItem->isExecuting){
        DisplayPercentDownloaded();
        return true;

    }else if(cachedFirmwareCheckResult==-1) { 
        sprintf(firmwareVersionStr, "Up to date %s", highestVersionOnAPI);
        //PrintToDisplay(F(menuItem->optName), String(firmwareVersionStr)  ); 

    } else if(cachedFirmwareCheckResult==1) { 
        char currentVersionTmp[16]; 
        char currentVersionStr[22]; 
        strncpy( currentVersionTmp, FIRMWARE_VERSION + 4, sizeof(FIRMWARE_VERSION) - 4 );
        sprintf(currentVersionStr, "Current %s", currentVersionTmp);
        sprintf(firmwareVersionStr, "Download %s >", highestVersionOnAPI);
        //PrintToDisplay(String(currentVersionStr), String(firmwareVersionStr)  ); 

    } else if(!WifiManager.WifiConnected()) {
        //PrintToDisplay(F(menuItem->optName), F("Connect to WiFi") ); 

    } else {
        //PrintToDisplay(F(menuItem->optName), F("Version not found") ); 
    }

    return false;
}
*/ 


void DisplayMessageAndRestart(const char* lineOneMessage,const char* lineTwoMessage){
    //SetHoldCurrentDisplay(true);
    //ShowLoading(false);
    //PrintToDisplay(lineOneMessage, lineTwoMessage );
    delay(3000);
    ESP.restart();
}


void HandleFatalError(const char* errorMsg){
    //Serial.print(shortErrorMsg);
    CT_LOGE(TAG, "HTTP EVENT ERROR: %s", errorMsg);
    //DisplayMessageAndRestart("Error", errorMsg ); 
}


//TaskHandle_t DownloadFirmwareTaskHandle;
void StartFirmwareDownload(){

    CT_LOGI(TAG, "StartFirmwareDownload Core: %d", xPortGetCoreID());
    
    //InactivityWatcherEnabled(false);

    // xTaskCreateUniversal(&UpdateFirmwareTask, "UpdateFirmwareTask", 1024 * 8, NULL, 5, NULL, BACKGROUND_TASK_CORE);

    httpRequest("GET", "/api/firmware/download/", NULL, &dummyHttpResponseHandler, &firmwareUpdateHttpError, &processFirmwareUpdateResponse);
}


void onDownloadProgress(size_t progress, size_t size) {
    ESP_LOGI(TAG, "onDownloadProgress: %d %d", progress, size);
    percComplete = (progress*100)/size;
    ESP_LOGI(TAG, "Percent Complete: %d%%",  percComplete);

    DisplayPercentDownloaded();

    vTaskDelay(1);
    //yield();
    //esp_task_wdt_reset();
}


void processFirmwareUpdateResponse(struct ResponseHeaderData headerData){
    CT_LOGI(TAG, "processFirmwareUpdateResponse");
    CT_LOGI(TAG, "headerData.contentLength: %ld", headerData.contentLength);
    CT_LOGI(TAG, "headerData.contentType: %s", headerData.contentType);

    if(headerData.contentLength<500000 || strcmp(headerData.contentType,"application/octet-stream")!=0){
        HandleFatalError("Invalid Response"); 
        return;
    } 

    WiFiClient* client = GetWifiClient();
    

    //remove leading \n so the magic first byte warning triggered  
    if(client->peek()==10){
        client->read();
    }

    //Update.setMD5(headerData.md5Check);

   //Update.setPreDownloadByteCheck(false);
    bool enoughSpace = Update.begin(headerData.contentLength);
    if(!enoughSpace){ 
        HandleFatalError("Not enough space"); 
        return;
    }

    client->setTimeout(10000);

    Update.onProgress(onDownloadProgress);

    size_t written = Update.writeStream(*client);

    if (written == headerData.contentLength) {
        CT_LOGI(TAG, "Written %d/%ld successfully", written, headerData.contentLength);
    } else {
        CT_LOGE(TAG, "Ended prematurely Written only : %d/%ld", written, headerData.contentLength);
        CT_LOGI(TAG, "%d", Update.getError());
        if(!WifiManager.WifiConnected())
            HandleFatalError("Lost WiFi connection"); 
        else 
            HandleFatalError("Save to flash failed"); 
    }

    if (Update.end()) {
        Serial.println("OTA done!");
        if (Update.isFinished()) {
            Serial.println("Update successfully completed. Rebooting.");
            DisplayMessageAndRestart("Firmware Downloaded","Restarting...");  
        } else {
            Serial.println("Update not finished? Something went wrong!");
            HandleFatalError("Download incomplete"); 
        }
    } else {
        Serial.println("Error Occurred in Update.end(). Error #: " + String(Update.getError()));
        HandleFatalError("Could not verify"); 
    }

}


/*void firmwareUpdateHttpSuccess(char* response){
    DisplayMessageAndRestart("Firmware Downloaded","Restarting...");  
}
*/

void firmwareUpdateHttpError(char* response){
    HandleFatalError(response);  
}