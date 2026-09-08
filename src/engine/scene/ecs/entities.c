#include "engine/types.h"
#include "engine/scene/ecs/ecs.h"


typedef struct Entity{
    uint16_t generation;
    uint16_t table;  //Which table the Entity belongs to
    uint32_t object; //The row within the table(above) that represents the entity
}Entity;

//Main ECS table
typedef struct Entities{
    Entity *list; //Sparse set
    uint32_t count;
    uint32_t capacity;
}Entities;



