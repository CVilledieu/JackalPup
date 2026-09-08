#include "engine/scene/ecs/ecs_config.h"
#include "engine/types.h"


typedef struct Entity{
    uint16_t generation;
    TableId table;  //Which table the Entity belongs to
    ObjectId object; //The row within the table
}Entity;


//Dense set of freed entity ids
typedef struct Recycler{
    EntityId* ids;
    uint32_t count;
    uint32_t capacity;
}Recycler;


//Sparse set tracking the relation from entity to object
typedef struct Entities{
    Entity* list;
    Recycler* freed;
    uint32_t count;
    uint32_t capacity;
}Entities;


static int CreateRecycler(Recycler* r){
    
}



int Entities_init(Entities* entities){
    
}

