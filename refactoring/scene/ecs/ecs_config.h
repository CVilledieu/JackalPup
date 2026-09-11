#ifndef ENGINE_SCENE_ECS_CONFIG_H
#define ENGINE_SCENE_ECS_CONFIG_H

#include "engine/scene/ecs/ecs.h"

typedef uint16_t ObjectId;
typedef uint16_t ComponentId;
typedef uint16_t TableId;


typedef struct GameObject{
    uint16_t generation;
    uint32_t entityId;  //Id within the larger sparse set of entities
}GameObject;


typedef struct Component{
    uint16_t size;      //Size of type
    uint16_t alignment; //Alignment of type
}Component;

typedef struct Entity{
    uint16_t generation;
    TableId table;  //Which table the Entity belongs to
    ObjectId object; //The row within the table
}Entity;



typedef struct ComponentRegistry{
    uint16_t count;
    Component components[MAX_COMPONENTS];
}ComponentRegistry;


typedef struct Column {
    void *data;
    ComponentId componentId;
    uint16_t elemSize;
    uint16_t alignment;
} Column;


typedef struct Table{
    uint16_t columnCount;
    Column *columns;

    uint16_t objectCount;
    uint16_t objectCapacity;
    GameObject *objects;
}Table;





#define REGISTER_COMPONENT(reg, T) \
    RegisterComponent((reg), sizeof(T), _Alignof(T))
    
ComponentId RegisterComponent(ComponentRegistry* reg, uint16_t size, uint16_t align);


//Entities
//========

//Matched array length to entities for simplicity during dev
//Once an entity is free their ID is added to Freed list to be reused
//Freed->next = 0 means Freed is empty
typedef struct Freed{
    EntityId* ids; //dense list
    uint32_t next;
}Freed;

//Sparse set tracking the relation from entity to object
typedef struct Entities{
    Entity* list;
    Freed recycle;
    uint32_t count;
    uint32_t capacity;
}Entities;

int Entities_init(Entities* ent, uint32_t cap);
EntityId NewEntity(Entities* ent);


#endif