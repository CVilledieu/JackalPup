#include "render/render_config.h"
#include "glad/glad.h"
#include "logging.h"

/* DEV NOTE:
    At the moment render module compiles parts of a shader program multiple times to form different programs. Meaning that if the number of shaders grows Consider compiling parts the once and then link them 

*/


//Called by BuildShader
//Compiles a single stage of a shader.
static uint32_t CompileEffect(GLenum stage, const ShaderDesc* desc, const char* body){
    uint32_t shader = glCreateShader(stage);
    if(!shader){
        LOG_ERROR("Unable to create shader id");
        return 0;
    }

    //Source order: #version, feature defines, then the stage body
    //src array length has a base of 2 to account for the only required parts: version and body
    const char* src[2 + SHADER_MAX_DEFINES];
    GLsizei count = 0;
    src[count++] = SHADER_VERSION;

    if(desc->defineCount > 0){
        for(int i = 0; i < desc->defineCount; ++i){
            src[count++] = desc->defines[i];
        }
    }

    src[count++] = body;

    glShaderSource(shader, count, src, NULL);
    glCompileShader(shader);
    
    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if(!ok){
        char buffer[1024];
        glGetShaderInfoLog(shader, (GLsizei)sizeof(buffer), NULL, buffer);
        LOG_ERROR("Shader compile failed.\n Shader Log:\n %s\n", buffer);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
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

