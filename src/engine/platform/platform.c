#include "platform.h"
#include "config.h"
#include "common/logging.h"
#include <stdio.h>
#include <stdlib.h>


#define ASSET_DIR "assets/"




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

char* ReadAssetFile(const char* fName){
    char path[512];
    int written = snprintf(path, sizeof(path), ASSET_DIR "%s", fName);
    if(written < 0 || (size_t)written >= sizeof(path)){
        LOG_ERROR("Asset path too long: %s", fName);
        return NULL;
    }

    FILE* file = fopen(path, "rb");
    if(!file){
        LOG_ERROR("Failed to open asset: %s", path);
        return NULL;
    }

    if(fseek(file, 0, SEEK_END) != 0){
        LOG_ERROR("Failed to seek asset: %s", path);
        fclose(file);
        return NULL;
    }

    long size = ftell(file);
    if(size < 0){
        LOG_ERROR("Failed to size asset: %s", path);
        fclose(file);
        return NULL;
    }
    rewind(file);

    char* buffer = malloc((size_t)size + 1);
    if(!buffer){
        LOG_ERROR("Failed to allocate %ld bytes for asset: %s", size, path);
        fclose(file);
        return NULL;
    }

    size_t read = fread(buffer, 1, (size_t)size, file);
    fclose(file);

    if(read != (size_t)size){
        LOG_ERROR("Short read on asset: %s", path);
        free(buffer);
        return NULL;
    }

    buffer[size] = '\0';
    return buffer;
}