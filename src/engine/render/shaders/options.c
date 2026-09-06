#include "render/render_config.h"
#include <stdio.h>
#include "common/logging.h"

typedef struct ShaderOptions{
    const char *const *options;
    uint8_t count;
}ShaderOptions;

typedef struct ShaderFile{
    GLenum stage;
    const char *fileName;
    const char *src;
}ShaderFile;


//Shader version data included on all shaders
#define SHADER_VERSION "#version 460 core \n"

#define SO_TRANSPARENT "TRANSPARENT_PASS"
#define SO_OPAQUE "OPAQUE_PASS"


#define ShaderOptionStr(op, val) "#define " op " " val "\n"
#define ShaderOptionVal(op, val) "#define " op " " TOSTRING(val) "\n"



static uint32_t CompileShaderObject(ShaderFile *sFile, ShaderOptions *sOptions){

    uint32_t obj = glCreateShader(sFile->stage);
    if(!obj){
        LOG_ERROR("Faied to create shader object ID");
        return 0;
    }

    const char *src[2 + MAX_OPTIONS];
    

    return obj;
}
