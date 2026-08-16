#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <stdlib.h>

#include "config.h"
#include "core/types.h"
#include "core/logging.h"


VkInstance instance = {0};


typedef struct Renderer{
    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice gpu;
    VkDevice device;
    uint32_t graphics_family;
    VkQueue graphics_queue;
    VkQueue current_queue;
}Renderer;



int renderer_init(Renderer* r, SDL_Window* window){
    volkInitialize();
    
    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .apiVersion = VK_API_VERSION_1_4,
        .pApplicationName = APP_NAME,
        .applicationVersion = APP_VERSION,
    };

    VkInstanceCreateInfo createInfo = { 
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
     };

    verify(vkCreateInstance(&createInfo, nullptr, &instance));
    
}

