#ifndef FirmwareUpdates_h
#define FirmwareUpdates_h

#include "Globals.h"
//#include "MenuSystem/MenuProgramDisplay.h"
//#include "MenuSystem/MenuSystem.h"
//#include "esp_http_client.h"
//#include "esp_https_ota.h"

bool PromptFirmwareUpdate();
bool FirmwareUpdateAvailable();
void SetHighestFirmwareAPIVersion(char* highestVersion);
//void FirmwareUpdateExecute(MenuItem* menuItem);
//bool FirmwareUpdateDisplay(MenuItem* menuItem);

void StartFirmwareDownload();

void processFirmwareUpdateResponse(struct ResponseHeaderData headerData);
//void firmwareUpdateHttpSuccess(char* response);
void firmwareUpdateHttpError(char* response);

#endif