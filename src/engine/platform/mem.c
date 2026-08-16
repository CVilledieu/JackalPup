#include "platform.h"
#include "types.h"
#include "logging.h"

#include <stdlib.h>

typedef struct mBlock{
    uint8_t* origin;
    uint8_t align; 
    size_t length;
}mBlock;


typedef struct Arena{
    size_t alignment;
    size_t position;
    size_t capacity;
    void* origin;
}Arena;

// Main allocator. Only used as a global variable within mem.c
typedef struct Allocator{
    uint16_t count;
    uint16_t capacity;
    Arena* list;
}Allocator;


static Allocator g_allocator = {0};


int MemoryAllocator_Init(size_t limit){
    g_allocator.count = 0;
    g_allocator.capacity = limit;
    g_allocator.list = (Arena*)malloc(sizeof(Arena) * limit);

    if(g_allocator.list == NULL){
        return 1;
    }
    return 0;
}


Arena* newArena(size_t limit, size_t alignment){

}


int alloc(size_t limit, size_t alignment, uint8_t* dest){
    
}