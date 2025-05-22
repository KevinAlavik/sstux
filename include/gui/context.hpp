#pragma once

#include <SDL2/SDL.h>

namespace SSTux::GUI
{
    bool InitOverlay();
    bool IsOverlayInitialized();
    SDL_Window *GetWindow();
    SDL_GLContext GetGLContext();
}
