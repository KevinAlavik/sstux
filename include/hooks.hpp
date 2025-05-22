#pragma once

#include <SDL2/SDL.h>

#ifdef __cplusplus
extern "C"
{
#endif
    void InstallSDLHooks(void);
    SDL_Window *GetStoredWindow(void);
#ifdef __cplusplus
}
#endif