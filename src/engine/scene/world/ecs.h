#ifndef ENGINE_WORLD_ECS_H
#define ENGINE_WORLD_ECS_H

typedef struct EntityReference{
    uint8_t generation;
    uint8_t table;
    uint32_t object;
}EntityReference;


typedef struct Entities{
    EntityReference* sparseSet;
    uint32_t count;
    uint32_t capacity;
}Entities;


typedef struct Table{
    uint8_t id;
    uint32_t objectCount;
    uint32_t columnHeaders;
    void* columns;
}Table;


typedef struct ECS{
    Entities* entities;
    Table* tables;
    uint32_t tableCount;    
}ECS;

#endif