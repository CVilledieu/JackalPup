//In place of parsing a file or the file system to aquire the names of the shader files.
//This file contains the shader file logic  
//All typing is contained within Render_Config.
//SFON.h should only be included in shaders.c and so should only contain direct data related to the creation of the shader effects

#ifndef SHADER_FILE_OBJECT_NOTATION_H
#define SHADER_FILE_OBJECT_NOTATION_H

#include "render/render_config.h" 


//Shader version data included on all shaders
#define SHADER_VERSION "#version 460 core \n"

//Options
#define TRANSPARENT_PASS "#define TRANSPARENT_PASS 1\n"
#define OPAQUE_PASS "#define OPAQUE_PASS 1\n"




typedef struct ShaderFile{
    const char* fName;
    ShaderTypes shaderType;
}ShaderFile;

//Shader File Object Notation (SFON)
static ShaderFile SFON[] = {
    {
        .fName = "world.glsl",
        .shaderType = VERTEX,
    },
    {
        .fName = "surface.glsl",
        .shaderType = FRAGMENT,
    },
};



#endif