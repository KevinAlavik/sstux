#pragma once

#include <SDL2/SDL.h>

namespace SSTux::Hooks
{
    void InstallSDLHooks();
    SDL_Window *GetStoredWindow();
    SDL_GLContext GetStoredGLContext();

    int GetSuperTuxMajor();
    int GetSuperTuxMinor();
    int GetSuperTuxPatch();
    bool HasValidSuperTuxVersion();
} // namespace SSTux::Hooks