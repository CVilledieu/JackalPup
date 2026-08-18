#include "render/shaders.h"
#include <stdint.h>
#include "glad/glad.h"
#include "logging.h"

static uint32_t CompileEffect(GLenum shaderType, const char* src){
    uint32_t shader = glCreateShader(shaderType);
    if(!shader){
        LOG_ERROR("Unable to create shader id");
        return 1;
    }

    glShaderSource(shader, 1, src, NULL);
    glCompileShader(shader);
    
    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if(!ok){
        char buffer[1024];
        glGetShaderInfoLog(shader, (GLsizei)sizeof(buffer), NULL, buffer);
        LOG_ERROR("Shader compile failed.\n Shader Log:\n %s\n", buffer);
        glDeleteShader(shader);
        return 1;
    }

    return shader;
}


uint32_t BuildShader(){
    //TODO: Verify parameters

    //FORK TODO: 
    // Left: Get shader data from src 
    // Right: Pass shader data in as a parameter
    uint32_t program = glCreateProgram();
    
    //FORK TODO:
    // LEFT: Hard code the attaching of the shaders parts (Effectively saying every shader using this function has that number of parts)
    // RIGHT: Dynamically track the number of shader parts and call glAttachShader/free for each of them.  
    glAttachShader(program,);
    

    glLinkProgram(program);

}


void DestroyEffect(uint32_t shader){
    if(!shader){
        return;
    }
    glDeleteProgram(shader);
}