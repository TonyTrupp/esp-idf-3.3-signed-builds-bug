#ifndef StringHelpers_h
#define StringHelpers_h

void GenerateRandomString(const char *alphanum, char *s, const int len, const int offset);
void GenerateRandomKey(char *s, const int len, const int offset);
void GenerateRandomNumberString(char *s, const int len, const int offset);
void strtrim(char* str);
int getPositionOf(const char *array, size_t size, char c);

#endif