
#include "modfunc.h"

void listDir(fs::FS& fs, const char* dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root) {
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()) {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file) {
        if(file.isDirectory()) {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels) {
                listDir(fs, file.path(), levels - 1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

bool matchWithWildcard(const char* text, const char* pattern, char& idstring)
{
    for(int i = 0;; ++i) {
        // If we reach the end of both strings, it's a match
        if(text[i] == '\0' && pattern[i] == '\0') return true;

        // If one reaches the end but not the other, it's a mismatch
        if(text[i] == '\0' || pattern[i] == '\0') return false;

        // If pattern has a '?' it accepts any character in text
        if(pattern[i] == '?') {
            idstring = text[i];
            continue;
        };

        // If the characters don't match exactly, return false
        if(text[i] != pattern[i]) return false;
    }
}
