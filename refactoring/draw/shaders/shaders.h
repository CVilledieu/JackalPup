#ifndef ENGINE_RENDER_SHADERS_H
#define ENGINE_RENDER_SHADERS_H

#include "render/types.h"

//Load every shader source body from disk once. Returns 1 on success, 0 on failure.
uint8_t InitShaderData(void);
//Release the source bodies loaded by InitShaderData; safe to call after all effects are built.
void FreeShaderData(void);
//Compile and link the requested effect, returning its program ID (0 on failure).
uint32_t CreateDrawEffect(ShaderEffect effect);
//Delete a program previously returned by CreateDrawEffect.
void DestroyEffect(uint32_t program);



#endif
