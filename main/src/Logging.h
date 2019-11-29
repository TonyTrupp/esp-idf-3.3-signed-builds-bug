#ifndef Logging_h
#define Logging_h

#include "Globals.h"

#define LOG_LEVEL_NONE    0 
#define LOG_LEVEL_ERROR   1
#define LOG_LEVEL_WARN    2
#define LOG_LEVEL_INFO    3
#define LOG_LEVEL_DEBUG   4
#define LOG_LEVEL_VERBOSE 5

void CT_LOGE(const char* tag,const char* msg, ...);
void CT_LOGW(const char* tag,const char* msg, ...);
void CT_LOGI(const char* tag,const char* msg, ...);
void CT_LOGD(const char* tag,const char* msg, ...);
void CT_LOGV(const char* tag,const char* msg, ...);
void CT_LOG_LINE();
void _CT_LOG_PRINT(short level, const char* tag, const char *format, va_list args);

#endif

#ifndef CT_LOG_LEVEL
#define CT_LOG_LEVEL LOG_LEVEL_DEBUG
#endif


