#pragma once

#include <SDL2/SDL.h>

#ifdef __cplusplus
extern "C"
{
#endif
    void InstallSDLHooks(void);
    SDL_Window *GetStoredWindow(void);
    SDL_GLContext GetStoredGLContext(void);
#ifdef __cplusplus
}
#endif