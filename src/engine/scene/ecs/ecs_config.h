#ifndef ENGINE_SCENE_ECS_CONFIG_H
#define ENGINE_SCENE_ECS_CONFIG_H

#include <stdint.h>
#include "engine/scene/ecs/ecs.h"


#define MAX_COMPONENTS 32
#define MAX_GAME_OBJECTS_TEMP 128

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

ComponentId RegisterComponent(ComponentRegistry* reg, uint16_t size, uint16_t align, const char* name);

#endif