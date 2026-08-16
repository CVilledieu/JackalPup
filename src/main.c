#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "platform.h"

int main(int argc, char *argv[]){
    //Init process
    SDL_Window* window;
    if(Platform_Init(window)){
        return 1;
    }


    //Run process


    //Shutdown
}

constexpr