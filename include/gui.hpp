#pragma once

#include <SDL2/SDL.h>

#ifdef __cplusplus
extern "C"
{
#endif
    void DrawGUI(SDL_Window *win);
#ifdef __cplusplus
}
#endif

void InitGUI();
bool IsGUIReady();