
#ifndef WifiManager_h
#define WifiManager_h

//#include "esp_wifi.h"
#include "src/Globals.h"
//#include "MenuSystem/MenuItem.h"

//Use the global WifiManager instance 
class WifiManagerClass {

  public:
    WifiManagerClass(); 
    void Reconnect(bool displayReconnect);
    void AttemptConnect(char* tmpNetworkName, char* tmpPassword);
    bool DisplayConnectToWifiStatus();
    void CantConnectMode();
    void EndCantConnectMode();
    const char* GetNetworkName();
    bool WifiConnected();
    bool WifiConnecting();
    void SetWifiConnecting(bool connecting);
    uint8_t WaitForConnectResult();
    void ForgetWifiNetwork();

  private:

    char* networkName = "unknown"; 
    char* password = NULL;

    bool _debug = true; 
    unsigned long connectTimeout = 15000;
    bool wifiConnecting = false;
    short int wifiStartConnectTime = 0;
    bool cantConnectMode = false;
    bool showReconnectStatus = true;

    //MenuItem* wifiCantConnectMenu;

    template <typename Generic>
    void DEBUG(Generic text);
};

//void ForgetWifiNetworkExecute(MenuItem* menuItem);

bool ConnectToWifiDisplay();

extern WifiManagerClass WifiManager;

#endif