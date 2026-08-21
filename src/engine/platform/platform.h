#ifndef ENGINE_PLATFORM_H
#define ENGINE_PLATFORM_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

int Platform_Init(SDL_Window* window);


//fNames should be relative to `assets/`
int ReadAssetFile(const char* fName, const char* dest);


#endif