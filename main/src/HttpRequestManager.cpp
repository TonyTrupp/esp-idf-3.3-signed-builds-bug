#include <WiFiClientSecure.h>
//#include "OLEDDisplay.h"
#include "WifiManager.h"
#include "HttpRequestManager.h"
//#include "PersistantStorage.h"
//#include "InactivityWatcher.h"
//#include "BatteryMonitor.h"
//#include "Provisioning.h"
#include "Helpers/StringHelpers.h"
#include "Logging.h"


static const char* TAG = "HttpRequestManager";

bool UseLocalServer = false;
bool UseSecureServer = true;

char*  serverDomain = "device.somedomain.com"; //"somedomain-device-api-qa.azurewebsites.net";  // Server URL
char*  serverHostHeader = "Host: www.azurewebsites.net";
char*  serverProtocol = "http";
short int serverPort = 443;
short unsigned int lostConnectionTimes = 0;

unsigned long lastConnectionTime = 0;   // last time you connected to the server, in milliseconds
const unsigned long connectionTimeoutSeconds = 30L; // seconds before the request fails
bool requestIsPending = false;
short requestCompletionStatus = 0; //1==success, -1==error 
char pendingErrorResponse[22];
#define PROGRAM_JSON_MAX_LENGTH 3500
char responseBuffer[PROGRAM_JSON_MAX_LENGTH] =  "";

HttpCallBackType httpSuccessCallback;
HttpCallBackType httpErrorCallback;
ProcessHttpCallbackType httpProcessCallback;


//Self Signed Certificate  
const char* test_root_ca= \
"-----BEGIN CERTIFICATE-----\n" \
"MIIGXzCCBUegAwIBAgIJAP2NDvI0rjgHMA0GCSqGSIb3DQEBCwUAMIGSMQswCQYD\n" \
"VQQGEwJVUzEPMA0GA1UECAwGT3JlZ29uMQ8wDQYDVQQHDAZPcmVnb24xFTATBgNV\n" \
"BAoMDENsb3VkVHJpZ2dlcjEhMB8GA1UEAwwYQ2xvdWRUcmlnZ2VyIERldmljZSBD\n" \
"ZXJ0MScwJQYJKoZIhvcNAQkBFhhTdXBwb3J0QENsb3VkVHJpZ2dlci5jb20wHhcN\n" \
"MTkxMDIwMDcyMTQ2WhcNMjkxMDE3MDcyMTQ2WjCBkjELMAkGA1UEBhMCVVMxDzAN\n" \
"BgNVBAgMBk9yZWdvbjEPMA0GA1UEBwwGT3JlZ29uMRUwEwYDVQQKDAxDbG91ZFRy\n" \
"aWdnZXIxITAfBgNVBAMMGENsb3VkVHJpZ2dlciBEZXZpY2UgQ2VydDEnMCUGCSqG\n" \
"SIb3DQEJARYYU3VwcG9ydEBDbG91ZFRyaWdnZXIuY29tMIIBIjANBgkqhkiG9w0B\n" \
"AQEFAAOCAQ8AMIIBCgKCAQEArTJhRzC4AcNn+8960ZXKuqAn59OvF7CaYlUh2IdW\n" \
"llaKRZ//5Bu1kFOpEQQpNX7BJ43KGtGfbG873zxzW7g8zyCz1ttNdKi2qufXri/Q\n" \
"uRAXUogu99ZaW9UAnId7lPqN0i5OGAikfynXlJCNOHEOjkwkL+NhgYgiglPBSalP\n" \
"O02jhwHHRnbHSycYxFjQUl3i1dEvaF3YMlePEMGJWpW63TSJwGYKm2gN1SKNR2zr\n" \
"xGFVhCJQHDAYEaHUsVto2/QCFja2CTt8IOA4+XPZbi0AvZe1WCKy+JRoKPHVvzHq\n" \
"FLDaiT5rCYevyW9+1Xyfuw8dYYPA+wO+9g6hmml1X/sRxQIDAQABo4ICtDCCArAw\n" \
"HQYDVR0OBBYEFNYleax+OwXyw1tmeEDE7PztE9x/MB8GA1UdIwQYMBaAFNYleax+\n" \
"OwXyw1tmeEDE7PztE9x/MAkGA1UdEwQCMAAwCwYDVR0PBAQDAgWgMIICBwYDVR0R\n" \
"BIIB/jCCAfqCCWxvY2FsaG9zdIIQY2xvdWR0cmlnZ2VyLmNvbYIXZGV2aWNlLmNs\n" \
"b3VkdHJpZ2dlci5jb22CGmRldmljZS1xYS5jbG91ZHRyaWdnZXIuY29tghtkZXZp\n" \
"Y2UtZGV2LmNsb3VkdHJpZ2dlci5jb22CH2RldmljZS1zdGFnaW5nLmNsb3VkdHJp\n" \
"Z2dlci5jb22CHGRldmljZS1kZW1vLmNsb3VkdHJpZ2dlci5jb22CF2RldmljZS5j\n" \
"bG91ZHRyaWdnZXIuY29tghIqLmNsb3VkdHJpZ2dlci5jb22CCTEyNy4wLjAuMYIM\n" \
"Y2xvdWR0cmlnZ2VyggwxOTIuMTY4LjAuMjOCCzE5Mi4xNjguMC4qgiljbG91ZHRy\n" \
"aWdnZXItZGV2aWNlLWFwaS5henVyZXdlYnNpdGVzLm5ldIIsY2xvdWR0cmlnZ2Vy\n" \
"LWRldmljZS1hcGktcWEuYXp1cmV3ZWJzaXRlcy5uZXSCLWNsb3VkdHJpZ2dlci1k\n" \
"ZXZpY2UtYXBpLWRldi5henVyZXdlYnNpdGVzLm5ldIIxY2xvdWR0cmlnZ2VyLWRl\n" \
"dmljZS1hcGktc3RhZ2luZy5henVyZXdlYnNpdGVzLm5ldIIuY2xvdWR0cmlnZ2Vy\n" \
"LWRldmljZS1hcGktZGVtby5henVyZXdlYnNpdGVzLm5ldDAsBglghkgBhvhCAQ0E\n" \
"HxYdT3BlblNTTCBHZW5lcmF0ZWQgQ2VydGlmaWNhdGUwHQYDVR0lBBYwFAYIKwYB\n" \
"BQUHAwEGCCsGAQUFBwMCMA0GCSqGSIb3DQEBCwUAA4IBAQB6u3hvJQRfNoZ7rEeN\n" \
"tX107RTvZm0nC8HKIG6M+VtyGrUsxMkIDo3WSRO/CzGwhrwCujPpXZ+RYOyQMcnR\n" \
"75a6c29tYiTopymbbMD+LLy7O92RKzYTuSDgMtNgZOGQ3P78svS1KTspZSho8TwN\n" \
"mIvrgwYe7zPrGdjLm5xacV532lk60IT5a4WsYydPv3a2QnOHzuQv81daKEieIVYM\n" \
"DYka27ptBZ5F3k8hYSS6BWwA0Xhxf1nZtIbmdGyRoKOVd3p+pBpB6G4i9z1KGjFq\n" \
"Vd3VEB6GuIy2DBa1aStwVI5Vfl5Iyb0+Wvs+CnHOaDOoqqYDaKEPqw+e/0JCjQ2/\n" \
"9NlQ\n" \
"-----END CERTIFICATE-----\n";

WiFiClient wifiClientInsecure;
WiFiClientSecure wifiClientSecure;
WiFiClient* wifiClientActive = !UseSecureServer ? &wifiClientInsecure : &wifiClientSecure;

/*WiFiClient* WifiClient(){  
  return UseLocalServer ? &wifiClientInsecure : &wifiClientSecure;
}*/

void setup_HttpRequestManager(){

  //if(isBatteryCriticallyLow()) return;

  CT_LOGD(TAG, "Setup_HttpRequestManager");
  CT_LOGI(TAG, "UseLocalServer = %d", UseLocalServer);
  CT_LOGI(TAG, "UseSecureServer = %d", UseSecureServer);
  
  //CT_LOGI(TAG, "WifiManager.GetNetworkName(): %s", WifiManager.GetNetworkName());
  
  //make sure QA mode is never deployed to other networks
  if(UseLocalServer){
    /*(strcmp(WifiManager.GetNetworkName(), "TBone") == 0 || 
     strcmp(WifiManager.GetNetworkName(), "iPhone") == 0) ){*/

    if( strcmp(WifiManager.GetNetworkName(), "iPhone") == 0 ){
      serverDomain = "172.20.10.4"; 
      serverHostHeader = "Host: 172.20.10.4";
    } else { 
      serverDomain = "192.168.0.23";
      serverHostHeader = "Host: 192.168.0.23";
    }

    Serial.print(F("serverDomain: "));Serial.println(serverDomain);
    serverPort = UseSecureServer ? 5001 : 5000;
  } else {
    UseLocalServer = false;
  }

  // SSL/TLS authentication to prevent spoofing of server 
  // To get a new root cert: 
  // openssl s_client -showcerts -connect www.somedomain.com:443 </dev/null
  if(serverPort == 443 || UseSecureServer){
    serverProtocol = "https";
    wifiClientSecure.setCACert(GetPemCert());
  }

  // SSL/TLS authentication to prevent spoofing of client
  //client.setCertificate(test_client_key); // for client verification
  //client.setPrivateKey(test_client_cert);	// for client verification
}


// this method makes a HTTP connection to the server:
void httpRequest(char* httpMethod, 
                 const char* path, 
                 char* data, 
                 HttpCallBackType successCallback, 
                 HttpCallBackType errorCallback, 
                 ProcessHttpCallbackType processCallback) {

  CT_LOG_LINE();
  CT_LOGD(TAG,"httpRequest()");

  CancelHttpRequest();

  //if( !strstr (path,"provisioning") )
  //  AbortProvisioning();

  lostConnectionTimes=0;

  httpSuccessCallback = successCallback;
  httpErrorCallback = errorCallback;
  httpProcessCallback = processCallback;


  requestIsPending = true;
  requestCompletionStatus = 0;
  lastConnectionTime = 0;

  // if there's a successful connection:
  CT_LOGD(TAG,"Connecting to server...");
  CT_LOGD(TAG,"Domain: %s",serverDomain);
  CT_LOGD(TAG,"Port: %d",serverPort);
  CT_LOGD(TAG,"Protocol: %s",serverProtocol);

  //handle dropped wifi connection 
  uint8_t status = WifiManager.WaitForConnectResult();
  if(status != WL_CONNECTED){
    httpErrorCallback("No wifi connection");
    return;
  }

  CT_LOGD(TAG,"Connected to WiFi");
  
  if (wifiClientActive->connect(serverDomain, serverPort)) {
    
    CT_LOGD(TAG,"Connected to server");

    //fail in code, not on client
    //wifiClientActive->setTimeout(connectionTimeoutSeconds+5);

    char requestHeaderURL[500]; 
    sprintf(requestHeaderURL,"%s %s://%s%s HTTP/1.0", httpMethod, serverProtocol, serverDomain, path); 

    CT_LOGD(TAG,"Making HTTP request to: ");
    CT_LOGI(TAG,"%s", requestHeaderURL);

    //CT_LOGD(TAG,"deviceGUID: %s", GetDeviceGUID());

    //Insecure, don't print key except for testing 
    //Serial.print("GetDeviceKey(): "); Serial.println(GetDeviceKey());

    // Make a HTTP request:
    //client->println("Host: www.azurewebsites.net");
    wifiClientActive->println(requestHeaderURL);
    wifiClientActive->println(serverHostHeader);
    wifiClientActive->println(F("User-Agent: ESP32WiFi"));
    wifiClientActive->println(F("Accept: */*"));
    wifiClientActive->println(F("Cache-Control: no-cache")); 

    //wifiClientActive->println(F("Accept-Encoding: gzip, deflate")); 
    //wifiClientActive->println(F("Connection: keep-alive")); 

    //device GUID is generated on device, device key is generated on API 
    //wifiClientActive->print("deviceKey: "); wifiClientActive->println(GetDeviceKey());
    //wifiClientActive->print("deviceGUID: "); wifiClientActive->println(GetDeviceGUID());
    //wifiClientActive->print("firmware: "); wifiClientActive->println(FIRMWARE_VERSION);
    wifiClientActive->print("DeviceInfo: "); wifiClientActive->println(GetDeviceInfo());
    //wifiClientActive->print("macID: "); wifiClientActive->println(GetMacID()); esp_efuse_mac_get_custom(uint8_t *mac) 
    
    //close header just means that the client is expecting you to close the connection after sending the response 
    wifiClientActive->println("Connection: close");

    //wifiClientActive->print("macID: "); wifiClientActive->println(GetMacID()); esp_efuse_mac_get_custom(uint8_t *mac) 
    

    if(data==nullptr) data = "{}";
    if( strcmp(httpMethod, "GET") != 0  ){
      wifiClientActive->println(F("Content-Type: application/json;"));

      CT_LOGI(TAG,"Post Data:");
      CT_LOGI(TAG,"%s", data);

      if(data[0]=='{')
        wifiClientActive->print(F("Content-Length: ")); wifiClientActive->println(strlen(data));

      wifiClientActive->println();
      wifiClientActive->print(data);
    } else {
      wifiClientActive->println();
    } 

    //wifiClientActive->println(F("Connection: close\r\n"));
    //wifiClientActive->print(F("\r\n"));

    // note the time that the connection was made:
    lastConnectionTime = millis();
    //RestartInactivityWatcher();
  } else {
    //RestartInactivityWatcher();

    // if you couldn't make a connection, this may be due to an expired certificate 
    // you can get a new one with this command: 
    // openssl s_client -showcerts -connect somedomain-device-api.azurewebsites.net:443 </dev/null 
    CT_LOGE(TAG, "Http Request Server Connection Failed");
    httpErrorCallback("Server Not Found");
    requestIsPending = false;
  }
}


String getHeaderValue(String header, String headerName) {
  return header.substring(strlen(headerName.c_str()));
}



void checkForHttpResponse(){

  if(!requestIsPending || lastConnectionTime==0) return;

  //RestartInactivityWatcher();

  if (millis() - lastConnectionTime > connectionTimeoutSeconds * 1000L) {
    CT_LOGW(TAG,"Connection Timeout");
    //PrintToDisplay("Http Request Timeout", "");
    setPendingErrorResponse("Connection Timeout");
    CancelHttpRequest();
    return; 
  }

  if(!WifiManager.WifiConnected()){
    CT_LOGW(TAG,"Lost Wifi Connection");
    setPendingErrorResponse("Lost Wifi Connection");
    CancelHttpRequest();
    return;
  }

  //problem if it's not connected with no data being recieved. If there's data in buffer, then connected will be true 
  if(!wifiClientActive->connected()){
    lostConnectionTimes++;
    CT_LOGD(TAG,"Lost Connection Times: %d", lostConnectionTimes);
    if(lostConnectionTimes<25) return; 
    CT_LOGW(TAG,"Cancelling HTTP Request");
    setPendingErrorResponse("Lost Connection");
    CancelHttpRequest();
    lostConnectionTimes=0;
    return;
  }

  if(!wifiClientActive->available()) return;

  CT_LOGD(TAG,"wifiClientActive has content available");

  //long contentLength = 0;
  //char contentType[50];
  //char statusCode[10];
  bool endOfHeadersFound = false;
  bool statusCodeFound = false;
  ResponseHeaderData headerData;

  //READ RESPONSE HEADERS  
  while (wifiClientActive->available()) {
    
    char line[255] = {0};
    wifiClientActive->readBytesUntil('\r', line, sizeof(line));

    //String lineStr = wifiClientActive->readStringUntil('\r');
    // remove space, to check if the line is end of headers
    //lineStr.trim();
    //lineStr.toCharArray(line, 255);

    //String line = wifiClientActive->readStringUntil('\n');
    strtrim(line);

    // if the the line is empty, this is end of headers 
    if (getPositionOf(line, sizeof(line), '\0')==1){
      CT_LOGD(TAG,"End Of Headers Found");
      endOfHeadersFound = true;
      break;   
    }

    // Check if the HTTP Response is 200 else break and handle as error 
    if (strncmp(line, "HTTP/1.", sizeof("HTTP/1.")-1) == 0) {
      statusCodeFound = true;
      strtok (line," ");  
      strncpy(headerData.statusCode, strtok (NULL, " "), 10);
      CT_LOGD(TAG,"Status Code: %s", headerData.statusCode);
      if (strcmp(headerData.statusCode,"200")!=0) {
        CT_LOGW(TAG,"Bad Response");
        if(strcmp(headerData.statusCode,"503")!=0)
          setPendingErrorResponse("Server Maintenance");
        else if(strcmp(headerData.statusCode,"403")!=0)
          setPendingErrorResponse("403 Forbidden");
        else if(strcmp(headerData.statusCode,"404")!=0)
          setPendingErrorResponse("404 Not Found");
        else if(strcmp(headerData.statusCode,"401")!=0)
          setPendingErrorResponse("401 Unauthorized");
        else if(strcmp(headerData.statusCode,"500")!=0)
          setPendingErrorResponse("500 Server Error");
        else 
          setPendingErrorResponse(headerData.statusCode);

        CancelHttpRequest();
        return;
      }else{
        CT_LOGD(TAG,"Ok Response");
        continue;
      }
    }else if(!statusCodeFound){
      setPendingErrorResponse("Missing HTTP status code");
      CancelHttpRequest();
      return;
    }

    if ( strstr (line,"Content-Length: ") ){
      headerData.contentLength = atol((getHeaderValue(line, "Content-Length: ")).c_str());
      CT_LOGD(TAG,"Content Length %l", headerData.contentLength);
      continue;
    }

    if ( strstr (line,"Content-Type: ") ){ 
      strtok (line," ");  
      strncpy(headerData.contentType, strtok (NULL, " "), 50);
      CT_LOGD(TAG,"Content Type: %s", headerData.contentType);
      continue;
    }

    if ( strstr (line,"x-MD5: ") ){ 
      strtok (line," ");  
      strncpy(headerData.md5Check, strtok (NULL, " "), 50);
      CT_LOGD(TAG,"md5Check: %s", headerData.md5Check);
      continue;
    }
    
    CT_LOGD(TAG, "%s", line);
  } 

  if (!endOfHeadersFound) {
    setPendingErrorResponse("No end of header");
    CancelHttpRequest();
    return;
  }

  //READ RESPONSE BODY 

  if(httpProcessCallback!=nullptr && httpProcessCallback!=0){
    CT_LOGD(TAG,"pre httpProcessCallback");
    httpProcessCallback(headerData);

  } else {

    short int bufferPos = 0;
    bool dataRecieved=false;
    while (requestIsPending && wifiClientActive->available()) {
      char c = wifiClientActive->read();
      responseBuffer[bufferPos] = c;
      bufferPos++;
      Serial.write(c);
      dataRecieved=true;
      //Serial.print(c);
    }
    responseBuffer[bufferPos] = '\0';

    if(dataRecieved){
      CT_LOGD(TAG,"");
      CT_LOG_LINE();
      CT_LOGD(TAG,"");
      wifiClientActive->stop();
      //Trigger success callback on primary UI loop thread 
      requestCompletionStatus=1;

    } else { 
      setPendingErrorResponse("No body content");
      CancelHttpRequest();
    }
  }
  
  requestIsPending = false;
}


//Kicks back to the main UI thread from the background thread  
void handleHttpResponseComplete(){

  if(requestCompletionStatus==0) return;

  //RestartInactivityWatcher();

  CT_LOGD(TAG,"handleHttpResponseComplete %d", requestCompletionStatus);

  if(requestCompletionStatus==1) 
    httpSuccessCallback(responseBuffer);

  if(requestCompletionStatus==-1) 
    handleInvalidResponse(pendingErrorResponse);

  requestCompletionStatus = 0;
  requestIsPending = false;
  lostConnectionTimes=0;

}


void setPendingErrorResponse(char* shortErrorMsg){
  CT_LOGW(TAG,"setPendingErrorResponse: %s", shortErrorMsg);
  strncpy(pendingErrorResponse,shortErrorMsg,strlen(pendingErrorResponse));
  requestCompletionStatus=-1;
}


void handleInvalidResponse(char* shortErrorMsg){
  CT_LOGW(TAG,"Invalid HTTP Response: %s", shortErrorMsg);
  //PrintToDisplay(F("Invalid HTTP Response"), shortErrorMsg);
  httpErrorCallback(shortErrorMsg);
}


void CancelHttpRequest(){

  CT_LOGD(TAG,"CancelHttpRequest()");

  lostConnectionTimes=0;
  requestIsPending = false;

  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  wifiClientActive->stop();
  // clean out the input buffer:
  wifiClientActive->flush();
}


bool httpRequestPending(){ 
  // Don't turn off the display while waiting for a network response 
  // Todo: might want to allow this for program update downloads eventually
  return requestIsPending;
}


const char* GetPemCert(){
  return test_root_ca;
}

const char* GetServerDomain(){
  return serverDomain; 
}

const char* GetServerProtocol(){
  return serverProtocol; 
}

short int GetServerPort(){
  return serverPort; 
}

WiFiClient* GetWifiClient(){
  return wifiClientActive; 
}

char deviceInfoStr[500]; 
char* GetDeviceInfo(){
  if(strlen(deviceInfoStr) != 0) 
    return deviceInfoStr;

  char currentVersionTmp[16];
  strncpy( currentVersionTmp, FIRMWARE_VERSION + 4, sizeof(FIRMWARE_VERSION) - 4 );

  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);

  //"deviceType|{appVersion}|{OS}|{OSVersion}|{deviceModel}|{deviceManufacturer}"
  sprintf(deviceInfoStr,"ct|%s|espidf|%s|%d|espressif", currentVersionTmp, esp_get_idf_version(), chip_info.revision); 
  return deviceInfoStr;
}


void dummyHttpResponseHandler(char* response){
    CT_LOGW(TAG,"dummyHttpResponseHandler");
    CT_LOGW(TAG,"%s", response);
}

