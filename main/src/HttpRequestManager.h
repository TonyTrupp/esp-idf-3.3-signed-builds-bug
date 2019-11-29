#ifndef HttpRequestManager_h
#define HttpRequestManager_h

#include "src/Globals.h"

struct ResponseHeaderData {
  long contentLength;
  char statusCode[10];
  char contentType[50];
  char md5Check[50];
};

typedef void (*HttpCallBackType)(char* response);
typedef void (*ProcessHttpCallbackType)(struct ResponseHeaderData headerData);

void setup_HttpRequestManager();
void httpRequest(char* httpMethod, const char* path, char* data, HttpCallBackType successCallback, HttpCallBackType errorCallback, ProcessHttpCallbackType processCallback=nullptr);
void checkForHttpResponse();
void handleHttpResponseComplete();
void CancelHttpRequest();
void setPendingErrorResponse(char* shortErrorMsg);
void handleInvalidResponse(char* shortErrorMsg);
bool httpRequestPending();
const char* GetPemCert();
const char* GetServerDomain();
const char* GetServerProtocol();
short int GetServerPort();
WiFiClient* GetWifiClient();
char* GetDeviceInfo();
void dummyHttpResponseHandler(char* response);

#endif