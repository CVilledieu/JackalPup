#include "engine/scene/ecs/ecs_config.h"


typedef struct ECS{
    Entities entities;
    Table* tables;
    uint16_t tableCount;
}ECS;


static ECS ecs = {0};


void ECS_init(uint32_t entityLimit, ){
    if(Entities_init(&ecs.entities, entityLimit)){
        exit(1);
    }

    ecs.tableCount = 0;
}
