#include "platform.h"
#include "config.h"




int Platform_Init(SDL_Window* window){
    if(SDL_InitSubSystem(SDL_INIT_VIDEO)){
        return 0;
    }

    window = SDL_CreateWindow(APP_NAME, 800, 600, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    if(!window){
        //Log err
        return 0;
    }

}

int ReadAssetFile(const char* fName, const char* dest){
    
}