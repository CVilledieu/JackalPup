/*
    InitShaderData is called at the start of the engine's init phase. It loads every
    shader source body from disk once, so the files only need to be opened one time.

    After that the engine calls CreateDrawEffect(effect) to compile and link the
    stages declared for that effect in shader_files.h.

    Once every effect has been built the source bodies can be released with
    FreeShaderData to reduce memory footprint.
*/

#include "render_config.h"
#include "render/shader_files.h"
#include "platform/platform.h"
#include "common/logging.h"
#include <stdlib.h>



#define MAX_OPTIONS 8


//Source bodies loaded from disk, indexed by ShaderBody. Owned by this module.
static char *shaderBodies[TOTAL_SHADER_BODIES] = {0};


//Loads shaderBodies with the source data from each file declared in shaderFiles.
uint8_t InitShaderData(void){
    for(int i = 0; i < TOTAL_SHADER_BODIES; i++){
        shaderBodies[i] = ReadAssetFile(shaderFiles[i].fileName);
        if(!shaderBodies[i]){
            LOG_ERROR("Failed to load shader source: %s", shaderFiles[i].fileName);
            FreeShaderData();
            return 0;
        }
    }
    return 1;
}


void FreeShaderData(void){
    for(int i = 0; i < TOTAL_SHADER_BODIES; i++){
        free(shaderBodies[i]);
        shaderBodies[i] = NULL;
    }
}


static uint32_t CompileShaderObject(const StageObject *object){
    if(object->count > MAX_OPTIONS){
        LOG_ERROR("Shader object requests %u options; max is %d", object->count, MAX_OPTIONS);
        return 0;
    }

    uint32_t obj = glCreateShader(shaderFiles[object->srcBody].stage);
    if(!obj){
        LOG_ERROR("Failed to create shader object ID");
        return 0;
    }

    //Source order: #version, feature defines, then the stage body
    //src array length has a base of 2 to account for the only required parts: version and body
    const char *src[2 + MAX_OPTIONS];
    GLsizei count = 0;
    src[count++] = SHADER_VERSION;

    for(uint8_t i = 0; i < object->count; i++){
        src[count++] = object->options[i];
    }

    src[count++] = shaderBodies[object->srcBody];

    glShaderSource(obj, count, src, NULL);
    glCompileShader(obj);

    GLint ok = 0;
    glGetShaderiv(obj, GL_COMPILE_STATUS, &ok);

    if(!ok){
        char buffer[1024];
        glGetShaderInfoLog(obj, (GLsizei)sizeof(buffer), NULL, buffer);
        LOG_ERROR("Shader object failed to compile.\n Log data: \n %s\n", buffer);
        glDeleteShader(obj);
        return 0;
    }

    return obj;
}


uint32_t CreateDrawEffect(ShaderEffect effect){
    if(effect >= SHADER_EFFECT_COUNT){
        LOG_ERROR("Requested effect %d is out of range", effect);
        return 0;
    }

    const ShaderEffectDef *def = &shaderEffects[effect];
    if(def->objectCount > MAX_OPTIONS){
        LOG_ERROR("Effect %d has %u stages; max is %d", effect, def->objectCount, MAX_OPTIONS);
        return 0;
    }

    uint32_t program = glCreateProgram();
    if(!program){
        LOG_ERROR("Failed to create shader program ID");
        return 0;
    }

    uint32_t objects[MAX_OPTIONS];
    uint8_t compiled = 0;

    for(uint8_t i = 0; i < def->objectCount; i++){
        objects[i] = CompileShaderObject(&def->objects[i]);
        if(!objects[i]){
            goto fail;
        }
        compiled++;
        glAttachShader(program, objects[i]);
    }

    glLinkProgram(program);

    GLint ok = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if(!ok){
        char buffer[1024];
        glGetProgramInfoLog(program, (GLsizei)sizeof(buffer), NULL, buffer);
        LOG_ERROR("Shader program failed to link.\n Log data: \n %s\n", buffer);
        goto fail;
    }

    //Objects are baked into the program once linked; the shader IDs are no longer needed.
    for(uint8_t i = 0; i < compiled; i++){
        glDetachShader(program, objects[i]);
        glDeleteShader(objects[i]);
    }

    return program;

fail:
    for(uint8_t i = 0; i < compiled; i++){
        glDetachShader(program, objects[i]);
        glDeleteShader(objects[i]);
    }
    glDeleteProgram(program);
    return 0;
}


void DestroyEffect(uint32_t program){
    if(program){
        glDeleteProgram(program);
    }
}