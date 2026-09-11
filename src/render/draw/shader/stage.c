#include "draw/shader/effects.h"
#include "glad/glad.h"
#include "logging.h"

//Shader version data included on all shaders
#define SHADER_VERSION "#version 460 core \n"
#define SHADER_COMPONENT_COUNT 3


static uint32_t CompileStage(GLenum stageType, const char* options, const char* body){
    uint32_t stageObject = glCreateShader(stageType);
    if(!stageObject){
        LOG_ERROR("Failed to create stage object");
        return 0;
    }

    const char* src[SHADER_COMPONENT_COUNT] = {SHADER_VERSION, options, body};

    glShaderSource(stageObject, SHADER_COMPONENT_COUNT, src, NULL);
    glCompileShader(stageObject);

    
    GLint ok = 0;
    glGetShaderiv(stageObject, GL_COMPILE_STATUS, &ok);

    if(!ok){
        char buffer[1024];
        glGetShaderInfoLog(stageObject, (GLsizei)sizeof(buffer), NULL, buffer);
        LOG_ERROR("Shader object failed to compile.\n Log data: \n %s\n", buffer);
        glDeleteShader(stageObject);
        return 0;
    }

    return stageObject;
}


uint32_t CreateEffect(SER* list, uint8_t count){
    if(!list || !count){
        return 0;
    }

    GLuint prog = glCreateProgram();

    for (int i = 0; i < count; ++i) {
        uint32_t stage = CompileStage(list[i].stageType, list[i].options, list[i].body);
        
        if(!stage){
            //Faulty program data 
            glDeleteProgram(prog);
            return 0;
        }

        glAttachShader(prog, stage);
        glDeleteShader(stage);   /* flagged now, freed once detached */
    }

    glLinkProgram(prog);
}