#ifndef RENDER_H
#define RENDER_H

#define AWAIT_GPU_TIMEOUT 1000000ULL 
#define FRAME_COUNT 3

#define DEFAULT_FRAME_COLOR 0.0f, 0.0f, 0.0f, 1.0f

void RenderEngine_init(void);
void RenderEngine_shutdown(void);

int Draw(void);

#endif