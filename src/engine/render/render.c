#include "render_config.h"
#include "glad/glad.h"
#include "shaders.h"
#include "platform/platform.h"



typedef struct DrawState{
    uint8_t current;
    GLsync frames[(size_t)FRAME_COUNT];
    uint32_t Seffects[(size_t)SHADER_COUNT];
}DrawState;


DrawState g_drawState = {0};


void RenderEngine_init(void){
    //Init frame statuses to zero value
    for (int i = 0; i < FRAME_COUNT; i++){
        g_drawState.frames[i] = NULL;
    }
    g_drawState.current = 0;

    char* worldFileData;
    char* surfaceFileData;

    ReadAssetFile(WORLD_SHADER_FILE, worldFileData);
    ReadAssetFile(SURFACE_SHADER_FILE, surfaceFileData);


    ShaderDesc transDesc = {
        .vertexSrc = worldFileData,
        .fragmentSrc = surfaceFileData,
        .defines[0] = "#define TRANSPARENT_PASS",
        .defineCount = 1,
    };
    ShaderDesc opaqueDesc = {
        .vertexSrc = worldFileData,
        .fragmentSrc = surfaceFileData,
        .defineCount = 0,
    };

    g_drawState.Seffects[TRANSPARENT] = BuildShader(&transDesc);
    g_drawState.Seffects[OPAQUE] = BuildShader(&opaqueDesc);

    
}


void RenderEngine_shutdown(void){
    for (int i = 0; i < SHADER_COUNT; i++){
        DestroyShader(g_drawState.Seffects[i]);
    }
}




static inline void ClearFrame(void){
    glClearColor(DEFAULT_FRAME_COLOR);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}



//
int Draw(void){
    //Verify gpu is done with the frame I want to write to
    GLenum frameStatus = 0;
    do{
        frameStatus = glClientWaitSync(g_drawState.frames[g_drawState.current], 0, AWAIT_GPU_TIMEOUT);
        if(frameStatus == GL_WAIT_FAILED){
            return 1; 
        }
    }while(frameStatus != GL_ALREADY_SIGNALED && frameStatus != GL_CONDITION_SATISFIED);
    glDeleteSync(g_drawState.frames[g_drawState.current]);

    ClearFrame();



    
    //Submit frame to GPU and move frame tracker forward
    g_drawState.frames[g_drawState.current] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    g_drawState.current = (uint8_t)((g_drawState.current + 1) % FRAME_COUNT);

    return 0;
}
