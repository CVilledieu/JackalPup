#ifndef SHADER_SOURCE_H
#define SHADER_SOURCE_H

typedef enum SSR{
    OPAQUE,
    TRANSPARENT,
    TOTOAL_SHADER_SRC_REF,
}SSR;

typedef struct ShaderFile{
    const char *fileName;
    const char *defines;
}ShaderFile;

typedef struct ShaderSrcRef{
    ShaderFile* fileList;
    int count;
}ShaderSrcRef;





#endif