#include "SFON.h"
#include "glad/glad.h"
#include "logging.h"


//Called by BuildShader
//Compiles a single module of a shader.
static uint32_t CompileModule(GLenum stage, const char* modSrc, const char** options, uint8_t optionCount){
    uint32_t module = glCreateShader(stage);
    if(!module){
        LOG_ERROR("Unable to create module id");
        return 0;
    }

    //Source order: #version, feature defines, then the stage body
    //src array length has a base of 2 to account for the only required parts: version and body
    const char* src[2 + MAX_OPTIONS];
    GLsizei count = 0;
    src[count++] = SHADER_VERSION;

    if(optionCount > 0){
        for(int i = 0; i < optionCount; i++){
            src[count++] = options[i];
        }
    }

    src[count++] = modSrc;
    glShaderSource(module, count, src, NULL);
    glCompileShader(module);

    GLint ok = 0;
    glGetShaderiv(module, GL_COMPILE_STATUS, &ok);
    if(!ok){
        char buffer[1024];
        glGetShaderInfoLog(module, (GLsizei)sizeof(buffer), NULL, buffer);
        LOG_ERROR("Shader compile failed.\n Shader Log:\n %s\n", buffer);
        glDeleteShader(module);
        return 0;
    }

    return module;

}

