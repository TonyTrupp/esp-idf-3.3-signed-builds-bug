#include "Logging.h"


void CT_LOGE(const char* tag, const char* msg, ...){
    va_list args;
    va_start(args, msg);
    _CT_LOG_PRINT(LOG_LEVEL_ERROR, tag, msg, args);
}


void CT_LOGW(const char* tag, const char* msg, ...){
    va_list args;
    va_start(args, msg);
    _CT_LOG_PRINT(LOG_LEVEL_WARN, tag, msg,args);
}


void CT_LOGI(const char* tag, const char* msg, ...){
    va_list args;
    va_start(args, msg);
    _CT_LOG_PRINT(LOG_LEVEL_INFO, tag, msg,args);
}

void CT_LOGD(const char* tag, const char* msg, ...){
    va_list args;
    va_start(args, msg);
    _CT_LOG_PRINT(LOG_LEVEL_DEBUG, tag, msg,args);
}


void CT_LOGV(const char* tag, const char* msg, ...){
    va_list args;
    va_start(args, msg);
    _CT_LOG_PRINT(LOG_LEVEL_VERBOSE, tag, msg,args);
}

const char* LOG_LINE = "\u001b[36m--------------------------------------------------------------------------------------\u001B[0m";
 void CT_LOG_LINE() {
    if( LOG_LEVEL_INFO > CT_LOG_LEVEL) return;
    Serial.println(LOG_LINE);
 }

bool _threadLoggingLock = false;
void _CT_LOG_PRINT(short level, const char* tag, const char *format, va_list args) {

     if( level > CT_LOG_LEVEL) return;

     while(_threadLoggingLock){
         //allow other thread to finish 
     }

     _threadLoggingLock = true;

    if(level==LOG_LEVEL_ERROR) 
        Serial.print("\u001b[31mE ");
    else if(level==LOG_LEVEL_WARN) 
        Serial.print("\u001b[33mW ");
    else if(level==LOG_LEVEL_INFO) 
        Serial.print("\u001b[32mI ");
    else if(level==LOG_LEVEL_DEBUG) 
        Serial.print("D ");
    else if(level==LOG_LEVEL_VERBOSE) 
        Serial.print("V ");

    if(tag!=nullptr && tag!='\0'){
        Serial.print(tag);
        Serial.print(": ");
    }

    /*
    "\u001b[1;31m Red message" );
    "\u001b[1;32m Green message" );
    "\u001b[1;33m Yellow message" );
    "\u001b[1;34m Blue message" );
    "\u001b[1;35m Purple message" );
    "\u001b[1;36m Cyan message" );
    */

    // loop through format string
    for (; *format != 0; ++format) {
        if (*format == '%') {
            ++format;
            if (*format == '\0') break;
            if (*format == '%') {
                Serial.print(*format);
                continue;
            }
            if( *format == 's' ) {
				char *s = (char *)va_arg( args, int );
				Serial.print(s);
				continue;
			}
            if( *format == 'd' || *format == 'i') {
				Serial.print(va_arg( args, int ),DEC);
				continue;
			}
            if( *format == 'x' ) {
				Serial.print(va_arg( args, int ),HEX);
				continue;
			}
            if( *format == 'X' ) {
				Serial.print("0x");
				Serial.print(va_arg( args, int ),HEX);
				continue;
			}
            if( *format == 'b' ) {
				Serial.print(va_arg( args, int ),BIN);
				continue;
			}
            if( *format == 'B' ) {
				Serial.print("0b");
				Serial.print(va_arg( args, int ),BIN);
				continue;
			}
            if( *format == 'l' ) {
				Serial.print(va_arg( args, long ),DEC);
				continue;
			}

            if( *format == 'c' ) {
				Serial.print(va_arg( args, int ));
				continue;
			}
            if( *format == 't' ) {
				if (va_arg( args, int ) == 1) {
					Serial.print("T");
				}
				else {
					Serial.print("F");				
				}
				continue;
			}
            if( *format == 'T' ) {
				if (va_arg( args, int ) == 1) {
					Serial.print("true");
				}
				else {
					Serial.print("false");				
				}
				continue;
			}

        }
        Serial.print(*format);
    }

    //reset color and new line 
    Serial.println("\u001B[0m");

    _threadLoggingLock = false;
 }



