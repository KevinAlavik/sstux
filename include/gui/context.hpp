#pragma once

#include <SDL2/SDL.h>

namespace SSTux::GUI
{
    struct OverlaySystemInfo
    {
        int windowWidth = 0, windowHeight = 0;
        SDL_version sdlVersion{};
        const char *glVersion = nullptr;
        const char *glVendor = nullptr;
        const char *glRenderer = nullptr;
        const char *glslVersion = nullptr;
    };

    extern OverlaySystemInfo overlayInfo;

    bool InitOverlay();
    bool IsOverlayInitialized();
    SDL_Window *GetWindow();
    SDL_GLContext GetGLContext();
}
