#include "my_custom.h"

void listDir(fs::FS& fs, const char* dirname, uint8_t levels);

bool matchWithWildcard(const char* text, const char* pattern);
