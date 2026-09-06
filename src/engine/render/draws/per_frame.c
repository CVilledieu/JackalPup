#include "render/draws/buffers.h"
#include "render/render_config.h"




//Creates a ring buffer for the global per frame data. 
int Globals_Init(Globals* globals){
    if(!globals){
        LOG_ERROR("Global handler wasnt passed in");
        return 0;
    }

    glGenBuffers(1, &globals->ubo);
    if(!globals->ubo){
        LOG_ERROR("Failed to create GLBuffer ID");
        return 0;
    }

    size_t bufferSize = sizeof(PerFrame) * FRAME_COUNT;

    glBindBuffer(GL_UNIFORM_BUFFER, globals->ubo);
    glBufferStorage(GL_UNIFORM_BUFFER, bufferSize, NULL, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
    globals->buffer = glMapBufferRange(GL_UNIFORM_BUFFER, 0, bufferSize, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

    if(!globals->buffer){
        glDeleteBuffers(1, globals->ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        LOG_ERROR("Failed to map PerFrame buffer");
        return 0;
    }

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return 1;
}

void Globals_Shutdown(Globals* globals){
    if(!globals || !globals->ubo){
        return;
    }
    glDeleteBuffers(1, globals->ubo);
}