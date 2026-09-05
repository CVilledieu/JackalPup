#include "render/render_config.h"




typedef struct ShaderAsset{
    const char *fName;
    GLenum stage;

}ShaderAsset;

typedef enum ShaderModules{
    
    TOTAL_SHADER_MODUELS
}ShaderModules;


static uint32_t CompileShaderModule[TOTAL_SHADER_MODUELS] = {
    []
};


static uint32_t CompileShaderStage(){
    
}