#include "render/render_config.h"
#include <stdio.h>
#include "common/logging.h"

typedef struct ShaderOptions{

}ShaderOptions;


//Shader version data included on all shaders
#define SHADER_VERSION "#version 460 core \n"

#define SO_TRANSPARENT "TRANSPARENT_PASS"
#define SO_OPAQUE "OPAQUE_PASS"


#define ShaderOptionStr(op, val) "#define " op " " val "\n"
#define ShaderOptionVal(op, val) "#define " op " " TOSTRING(val) "\n"