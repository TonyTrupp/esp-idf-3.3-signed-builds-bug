
#include "WifiManager.h"
#include <WiFi.h>
//#include "OLEDDisplay.h"
//#include "InactivityWatcher.h"
//#include "Provisioning.h"
//#include "PersistantStorage.h"
//#include "RemoteWifiSetup.h"
//#include "MenuSystem/MenuProgramDisplay.h"
//#include "MenuSystem/MenuSystem.h"
//#include "MenuSystem/MenuTree.h"
//#include "BatteryMonitor.h";
//to slow wifi check-ins for power savings
#include "esp_wifi.h" 
#include "Logging.h"

 
bool demoIPhoneConnectMode = false;


WifiManagerClass::WifiManagerClass() { 
  DEBUG(F("WiFiManager Initiated"));
}


void WifiManagerClass::AttemptConnect(char* tmpNetworkName, char* tmpPassword){

  //ShowLoading(true);
  SetWifiConnecting(true);

  WiFi.enableSTA(true);

  if(tmpPassword!=NULL && strlen(tmpPassword)!=0){
    WiFi.begin(tmpNetworkName, tmpPassword);
  } else {
    WiFi.begin(tmpNetworkName);
  }

  WifiManager.WaitForConnectResult();

  //ShowLoading(false);
  SetWifiConnecting(false);

  if(WiFi.status() == WL_CONNECTED) { 
    networkName = tmpNetworkName;
    password = tmpPassword;
    
    EndCantConnectMode();
  }
}


void WifiManagerClass::Reconnect(bool displayReconnect){

  //if(isBatteryCriticallyLow()) return;

  Serial.println("WifiManagerClass::Reconnect"); 

  Serial.println("Reconnecting to Wifi...");

  wifiStartConnectTime = millis();

  showReconnectStatus = displayReconnect;
  Serial.print("WifiManagerClass::Reconnect showReconnectStatus: ");Serial.println(showReconnectStatus); 
  EndCantConnectMode();
  SetWifiConnecting(true);
  //ShowLoading(true);

  //show this fast only for startup process responsiveness.  
  //If another screen replaces the wifi connect at startup, this can be moved down or removed 
  DisplayConnectToWifiStatus();
  //UpdateDisplay();

  Serial.print("WifiManager.GetNetworkName: ");Serial.println(networkName);

  WiFi.enableSTA(true);

  /*
  if(password!=NULL && strlen(password)!=0){
    WiFi.begin(networkName, password);
  } else if(networkName!=NULL && strlen(networkName)!=0) {
    WiFi.begin(networkName);
  } else {*/
    //use last credential saved before reset 
    Serial.print(F("Reconnect to wifi with prior credentials"));

    if(demoIPhoneConnectMode){
      Serial.print(F("Connecting to TonysiPhone"));
      WiFi.begin("TonysiPhone", "mcscreedle");
    }else{
      WiFi.begin(); 
    }
  //}

  //slow down wifi check-in frequency to save power, only about once per 10 seconds instead of around 3 seconds. Only possible on station mode.  
  //the interval can be slowed down further with esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) 
  esp_wifi_set_ps(WIFI_PS_MAX_MODEM);
}


bool WifiManagerClass::DisplayConnectToWifiStatus(){

  if((WifiConnected() && !WifiConnecting()) || cantConnectMode) return false; 

  //if(IsProvisioningRequired() && !ShouldProvisionInBackground()) showReconnectStatus=true;

  if (!WifiConnected()) { 

    //reconnect if the connection was dropped 
    if(!WifiConnecting() && !cantConnectMode) Reconnect(showReconnectStatus);

    if(wifiStartConnectTime+(15*1000) < millis()){
      //RestartInactivityWatcher();
      //ShowLoading(false);
      //PrintToDisplay(F("No Wifi Connection"), F("Check your network")); 
      //SetHoldCurrentDisplay(true,3,true);
      SetWifiConnecting(false);

      CantConnectMode();
      //RestartInactivityWatcher();

      return true;

    } else { 
      //RestartInactivityWatcher();

      if(showReconnectStatus ){
        Serial.println("WifiManagerClass::DisplayConnectToWifiStatus showReconnectStatus");
        //ShowLoading(true);
        //PrintToDisplay(F("Connecting to Wifi"), F("")); 
        return true;
      }

      return false;
    }
  } else { 
    showReconnectStatus = false;
    //ShowLoading(false);
    SetWifiConnecting(false);
    EndCantConnectMode();
    Serial.println(F("Connected to Wifi!")); //Serial.println(String(WiFi.localIP()));
    return false;
  }
}

/*
void WifiManagerReconnectExecute(MenuItem* menuItem){
  Serial.println("WifiManagerReconnectExecute");
  WifiManager.Reconnect(true);
}
*/

//Used by both forget password settings option and reset device settings. 
//Must call PersistantDataCommit after, but only so many writes available so don't call it twice on reset. 
void WifiManagerClass::ForgetWifiNetwork(){
  WiFi.disconnect(true, true);
  //SetAutoReconnect(false);
  //PersistantDataCommit();
}

/*
void ForgetWifiNetworkExecute(MenuItem* menuItem){ 
  WifiManager.ForgetWifiNetwork();
  PersistantDataCommit();
  PrintToDisplay(F("Wifi Network Cleared"), F("New Network Required")); 
  delay(3000);
  RemoteWifiSetupStart();
}
*/

/*
void WifiManagerClass::CantConnectMode(){

  if(wifiCantConnectMenu == nullptr){
    RestartInactivityWatcher();
    wifiCantConnectMenu = new MenuItem("No Wifi Access");
    MenuItem* wifiTryAgain= new TriggerMenuItem("Try again");
    wifiTryAgain->setExecuteCallback(&WifiManagerReconnectExecute);
    MenuItem* setupNewNetwork= new TriggerMenuItem("Setup New Network");
    setupNewNetwork->setExecuteCallback(&RemoteWifiSetupExecute);
    //menuItem* wifiForgetNetworks= new TriggerMenuItem("Forget Wifi Networks");

    MenuItem* cantConnectOptions[] = {wifiTryAgain,setupNewNetwork};
    wifiCantConnectMenu->setSubOptions(2,cantConnectOptions);
  }

  cantConnectMode = true;

  SetCurrentMenuItem(*wifiCantConnectMenu,true);
}
*/ 


void WifiManagerClass::EndCantConnectMode(){
  if(!cantConnectMode) return;  
  Serial.println("WifiManagerClass::EndCantConnectMode");
  cantConnectMode = false;
  //RestoreRootMenu();
}



const char* WifiManagerClass::GetNetworkName(){
  String str = WiFi.SSID(); //networkName;
  if(str!=nullptr) return str.c_str();
  const char * emptyStr = new char(0);
  return emptyStr;
}


void WifiManagerClass::SetWifiConnecting(bool connecting){
  wifiConnecting = connecting;
}


bool WifiManagerClass::WifiConnecting(){
  return wifiConnecting;
}

bool WifiManagerClass::WifiConnected(){
  return WiFi.status() == WL_CONNECTED;
}


uint8_t WifiManagerClass::WaitForConnectResult() {

  // https://github.com/tzapu/WiFiManager/blob/master/WiFiManager.cpp 

  DEBUG (F("Waiting for connection result with time out"));
  unsigned long start = millis();
  bool keepConnecting = true;
  uint8_t status;
  while (keepConnecting) {
    
    status = WiFi.status();
    //Serial.println("WaitForConnectResult wifi status: ");Serial.println(status);
    if (millis() > start + connectTimeout) {
      keepConnecting = false;
      DEBUG (F("Connection timed out"));
    }
    if (status == WL_CONNECTED || status == WL_CONNECT_FAILED) {
      DEBUG (F("Exiting WaitForConnectResult"));
      keepConnecting = false;

      if (status == WL_CONNECTED && cantConnectMode) EndCantConnectMode();
      break;
    }

    /*if( keepConnecting && ( !provisioningCheckInPending() || !ShouldProvisionInBackground() ) ){
      //Serial.println("WifiManagerClass::WaitForConnectResult keepConnecting");
      //Serial.print("provisioningCheckInPending: "); Serial.println(provisioningCheckInPending());
      //sSerial.print("ShouldProvisionInBackground: "); Serial.println(ShouldProvisionInBackground());
      //ShowLoading(true);
      //PrintToDisplay(F("Connecting to Wifi") );
    }*/

    delay(10);
  }

  return status;
}




template <typename Generic>
void WifiManagerClass::DEBUG(Generic text) {
  if (_debug) {
    Serial.print("DEBUG_REMOTE_WIFI: "); Serial.println(text);
  }
}


//make global instance 
WifiManagerClass WifiManager;


bool ConnectToWifiDisplay(){
  return WifiManager.DisplayConnectToWifiStatus();
}

