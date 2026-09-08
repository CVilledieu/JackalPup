#include "engine/scene/ecs/ecs_config.h"
#include <stdlib.h>



ComponentId RegisterComponent(ComponentRegistry* reg, uint16_t size, uint16_t align, const char* name){}

static void *aligned_column_alloc(uint16_t align, uint16_t elemSize, uint32_t capacity){
    size_t bytes = (size_t) elemSize * capacity;
    
    if(align < sizeof(void*)) {
        align = sizeof(void*);
    }
    bytes = (bytes + align - 1) & ~((size_t)align - 1);//Round up
    return aligned_alloc(align, bytes); 
}


void CreateTalbe(Table* table, const ComponentRegistry* reg, const ComponentId* ids, uint16_t columnCount, uint32_t capacity){
    table->columnCount = columnCount;
    table->objectCount = 0;
    table->objectCapacity = capacity;

    table->columns = malloc(sizeof(Column) * columnCount);
    table->objects = malloc(sizeof(GameObject) * capacity);

    if(!table->columns || !table->objects){
        free(table->columns);
        free(table->objects);
    }

    for (uint16_t i = 0; i < columnCount; i++){
        Component com = reg->components[ids[i]];
        Column *col = &table->columns[i];
        col->componentId = ids[i];
        col->elemSize = com.size;
        col->alignment = com.alignment;
        col->data = aligned_column_alloc(com.alignment, com.size, capacity);
    }
}