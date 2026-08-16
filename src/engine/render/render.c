#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <stdlib.h>

#include "config.h"
#include "core/types.h"
#include "core/logging.h"


VkInstance instance = {0};

int Render_Init(void){
    if(SDL_InitSubSystem(SDL_INIT_VIDEO)){}
}

static void create_instance(App *app){
    uint32_t extension_count = 0;
    const char* const* extensions = SDL_Vulk
}


int VkAPP(void){
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