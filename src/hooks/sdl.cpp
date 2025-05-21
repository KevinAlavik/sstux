#include <stdio.h>
#include <dlfcn.h>
#include <SDL2/SDL.h>
#include <gui.hpp>
#include <imgui/imgui_impl_sdl2.h>

/* SDL Hooks */
typedef SDL_Window *(*SDL_CreateWindow_t)(const char *, int, int, int, int, Uint32);
typedef void (*SDL_SetWindowTitle_t)(SDL_Window *, const char *);
typedef void (*SDL_GL_SwapWindow_t)(SDL_Window *);
typedef int (*SDL_PollEvent_t)(SDL_Event *);
typedef SDL_GLContext (*SDL_GL_CreateContext_t)(SDL_Window *);

/* SDL Real Handlers */
static SDL_CreateWindow_t real_SDL_CreateWindow = NULL;
static SDL_SetWindowTitle_t real_SDL_SetWindowTitle = NULL;
static SDL_GL_SwapWindow_t real_SDL_GL_SwapWindow = NULL;
static SDL_PollEvent_t real_SDL_PollEvent = NULL;
static SDL_GL_CreateContext_t real_SDL_GL_CreateContext = NULL;

/* Global SDL variables used by SSTux */
static SDL_Window *g_Window = NULL;
static SDL_GLContext g_GLContext = NULL;

/* Init */
extern "C" void
InstallSDLHooks(void)
{
    real_SDL_CreateWindow = (SDL_CreateWindow_t)dlsym(RTLD_NEXT, "SDL_CreateWindow");
    real_SDL_SetWindowTitle = (SDL_SetWindowTitle_t)dlsym(RTLD_NEXT, "SDL_SetWindowTitle");
    real_SDL_GL_SwapWindow = (SDL_GL_SwapWindow_t)dlsym(RTLD_NEXT, "SDL_GL_SwapWindow");
    real_SDL_PollEvent = (SDL_PollEvent_t)dlsym(RTLD_NEXT, "SDL_PollEvent");
    real_SDL_GL_CreateContext = (SDL_GL_CreateContext_t)dlsym(RTLD_NEXT, "SDL_GL_CreateContext");

    if (!real_SDL_CreateWindow || !real_SDL_SetWindowTitle || !real_SDL_GL_SwapWindow || !real_SDL_PollEvent || !real_SDL_GL_CreateContext)
        fprintf(stderr, "[SSTux] Error: Failed to resolve one or more SDL functions\n");
}

/* Hooks */
extern "C" SDL_Window *SDL_CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags)
{
    if (!real_SDL_CreateWindow)
        return NULL;

    char modifiedTitle[512];
    snprintf(modifiedTitle, sizeof(modifiedTitle), "%s%s", title, " | SSTux ");
    SDL_Window *win = real_SDL_CreateWindow(modifiedTitle, x, y, w, h, flags);

    g_Window = win;
    fprintf(stderr, "[SSTux] Hooked SDL_CreateWindow, patched window title, stored window: %p\n", g_Window);

    return win;
}

extern "C" void SDL_SetWindowTitle(SDL_Window *window, const char *title)
{
    if (!real_SDL_SetWindowTitle)
        return;

    char modifiedTitle[512];
    snprintf(modifiedTitle, sizeof(modifiedTitle), "%s%s", title, " | SSTux ");

    g_Window = window;
    fprintf(stderr, "[SSTux] Hooked SDL_SetWindowTitle, patched window title, updated window: %p\n", g_Window);

    real_SDL_SetWindowTitle(window, modifiedTitle);
}

extern "C" void SDL_GL_SwapWindow(SDL_Window *window)
{
    if (!real_SDL_GL_SwapWindow)
        return;

    if (window == g_Window && SDL_GL_GetCurrentContext() != NULL)
    {
        DrawGUI(window);
    }

    real_SDL_GL_SwapWindow(window);
}

extern "C" int SDL_PollEvent(SDL_Event *event)
{
    if (!real_SDL_PollEvent)
        return 0;

    int ret = real_SDL_PollEvent(event);
    if (event != NULL && IsGUIReady())
    {
        ImGui_ImplSDL2_ProcessEvent(event);
        fprintf(stderr, "[SSTux] ImGui processed SDL2 event...\n");
    }

    return ret;
}

extern "C" SDL_GLContext SDL_GL_CreateContext(SDL_Window *window)
{
    if (!real_SDL_GL_CreateContext)
        return 0;

    SDL_GLContext c = real_SDL_GL_CreateContext(window);
    fprintf(stderr, "[SSTux] Made OpenGL context for window: %p\n", window);
    g_GLContext = c;
    return c;
}

/* Utils */
extern "C" SDL_Window *GetStoredWindow(void)
{
    return g_Window;
}

extern "C" SDL_GLContext GetStoredGLContext(void)
{
    return g_GLContext;
}