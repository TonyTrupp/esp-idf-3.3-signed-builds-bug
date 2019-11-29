#include "WiFi.h"
#include "../Globals.h"
#include "StringHelpers.h"


void GenerateRandomKey(char *s, const int len, const int offset) {
    //excluding 0 & O since they're confused
    static const char alphanum[] =
        "123456789"
        "ABCDEFGHIJKLMNPQRSTUVWXYZ"
        "abcdefghijklmnpqrstuvwxyz";
    GenerateRandomString(alphanum, s, len, offset);
}

void GenerateRandomNumberString(char *s, const int len, const int offset) {
    //excluding 0 since it's confused with O
    static const char numericChars[] = "123456789"; 
    GenerateRandomString(numericChars, s, len, offset);
}

void GenerateRandomString(const char *allowedChars, char *s, const int len, const int offset) {
    for (short int i = 0; i < len; ++i) {
        s[i+offset] = allowedChars[rand() % (strlen(allowedChars) - 1)];
    } 
    s[len+offset] = 0;
}


void strtrim(char* str) {
    int start = 0; // number of leading spaces
    char* buffer = str;
    while (*str && *str++ == ' ') ++start;
    while (*str++); // move to end of string
    int end = str - buffer - 1; 
    while (end > 0 && buffer[end - 1] == ' ') --end; // backup over trailing spaces
    buffer[end] = 0; // remove trailing spaces
    if (end <= start || start == 0) return; // exit if no leading spaces or string is now empty
    str = buffer + start;
    while ((*buffer++ = *str++));  // remove leading spaces: K&R
}

int getPositionOf(const char *array, size_t size, char c)
{
     const char* end = array + size;
     const char* match = std::find(array, end, c);
     return (end == match)? -1 : (match-array);
}


