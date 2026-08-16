#include "types.h"
#include "logging.h"
#include "world/ecs.h"



Entities* CreateECS(size_t limit){
    Entities* entities = malloc(sizeof(entities));
    if(entities == NULL){
        return NULL;
    }

    entities->sparseSet = malloc(sizeof(EntityReference) * limit);
    if(entities->sparseSet == NULL){
        free(entities);
        return NULL;
    }

    entities->count = 0;    
    entities->capacity = (uint32_t)limit;

    return entities;
}



int Entities_Push(){}