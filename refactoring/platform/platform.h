#ifndef ENGINE_PLATFORM_H
#define ENGINE_PLATFORM_H


//Reads an asset file whole and returns a malloc'd, NUL-terminated buffer the
//caller must free. fName is relative to assets/. Returns NULL on failure.
char* ReadAssetFile(const char* fName);


#endif