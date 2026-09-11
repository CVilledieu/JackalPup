#include "draw/state.h"


struct Render{
    DrawState drawState;
};

static struct Render sgRender = {0};


void init(void){
    if(DrawState_init(&sgRender.drawState)){
        exit(1); //Crit err exit
    }
}