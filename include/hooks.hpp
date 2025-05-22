#pragma once

#include <SDL2/SDL.h>

namespace SSTux::Hooks
{
    void InstallSDLHooks();
    SDL_Window *GetStoredWindow();
    SDL_GLContext GetStoredGLContext();
} // namespace SSTux::Hooks