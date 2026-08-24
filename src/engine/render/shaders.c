#include "SFON.h"
#include "glad/glad.h"
#include "logging.h"



// PROTOTYPING BELOW! CODE NOT CURRENTLY IN USE
//  Checking to see if shaders.c should build a list of ShaderPrograms to be sent back to the renderer
//  ATM BuildShader compiles all required shaders and then Links them to a program regardless if the shader had been built before
//  NEW APPROACH: Build shaders by parts. Create all shader parts once then link based on ShaderRef requests




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



int BuildShader(){

}





SEffect BuildShader(const ShaderDesc* desc){
    if(!desc || desc->defineCount < 0 || desc->defineCount > SHADER_MAX_DEFINES){
        LOG_ERROR("Invalid shader descriptor");
        return 0;
    }

    const struct { GLenum type; const char* body; } stages[] = {
        { GL_VERTEX_SHADER,   desc->vertexSrc },
        { GL_FRAGMENT_SHADER, desc->fragmentSrc },
        { GL_GEOMETRY_SHADER, desc->geometrySrc },
        { GL_COMPUTE_SHADER,  desc->computeSrc },
    };
    const int stageCount = (int)(sizeof(stages) / sizeof(stages[0]));

    SEffect program = glCreateProgram();
    if(!program){
        LOG_ERROR("Unable to create shader program");
        return 0;
    }

    uint32_t shaders[sizeof(stages) / sizeof(stages[0])] = {0};
    int shaderCount = 0;
    for(int i = 0; i < stageCount; ++i){
        if(!stages[i].body){
            continue;
        }
        uint32_t s = CompileEffect(stages[i].type, desc, stages[i].body);
        if(!s){
            for(int j = 0; j < shaderCount; ++j){
                glDeleteShader(shaders[j]);
            }
            glDeleteProgram(program);
            return 0;
        }
        shaders[shaderCount++] = s;
        glAttachShader(program, s);
    }

    if(shaderCount == 0){
        LOG_ERROR("Shader descriptor has no stages");
        glDeleteProgram(program);
        return 0;
    }

    glLinkProgram(program);

    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);

    //Once linked the stage objects are no longer needed by the program
    for(int i = 0; i < shaderCount; ++i){
        glDetachShader(program, shaders[i]);
        glDeleteShader(shaders[i]);
    }

    if(!linked){
        char buffer[1024];
        glGetProgramInfoLog(program, (GLsizei)sizeof(buffer), NULL, buffer);
        LOG_ERROR("Shader link failed.\n Program Log:\n %s\n", buffer);
        glDeleteProgram(program);
        return 0;
    }

    return program;
}


