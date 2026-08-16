#include "types.h"
#include "logging.h"
#include "platform.h"




typedef struct EntityReference{
    uint8_t generation;
    uint8_t table;
    uint32_t object;
}EntityReference;


typedef struct ECS{
    EntityReference* sparseSet;
    uint32_t count;
    uint32_t capacity;
}ECS;



ECS* CreateECS(size_t limit){
    ECS* ecs = malloc(sizeof(ECS));
    if(ecs == NULL){
        return NULL;
    }

    ecs->sparseSet = malloc(sizeof(EntityReference) * limit);
    if(ecs->sparseSet == NULL){
        free(ecs);
        return NULL;
    }

    ecs->count = 0;    
    ecs->capacity = (uint32_t)limit;

    return ecs;
}



int Entities_Push()