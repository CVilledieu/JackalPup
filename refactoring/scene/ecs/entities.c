#include "engine/scene/ecs/ecs_config.h"
#include "common/logging.h"




EntityId NewEntity(Entities* ent){

}


int Entities_init(Entities* ent, uint32_t cap){
    Freed* f = &ent->recycle;
    
    f->next = 0;
    ent->count = 0;
    ent->capacity = cap;

    f->ids = malloc(sizeof(EntityId) * cap);
    ent->list = malloc(sizeof(Entity) * cap);

    if(!f->ids || !ent->list){
        LOG_ERROR("Falled to allocate entity list");
        return 1;        
    }

    return 0;
}

