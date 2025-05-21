#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <SDL2/SDL.h>
#include <gui.hpp>

/* SDL Hooks */
typedef SDL_Window *(*SDL_CreateWindow_t)(const char *, int, int, int, int, Uint32);
typedef void (*SDL_SetWindowTitle_t)(SDL_Window *, const char *);
typedef void (*SDL_GL_SwapWindow_t)(SDL_Window *);

/* SDL Real Handlers */
static SDL_CreateWindow_t real_SDL_CreateWindow = NULL;
static SDL_SetWindowTitle_t real_SDL_SetWindowTitle = NULL;
static SDL_GL_SwapWindow_t real_SDL_GL_SwapWindow = NULL;

/* Global SDL variables used by SSTux */
static SDL_Window *g_Window = NULL;

/* Init */
void InstallSDLHooks(void)
{
    real_SDL_CreateWindow = (SDL_CreateWindow_t)dlsym(RTLD_NEXT, "SDL_CreateWindow");
    real_SDL_SetWindowTitle = (SDL_SetWindowTitle_t)dlsym(RTLD_NEXT, "SDL_SetWindowTitle");
    real_SDL_GL_SwapWindow = (SDL_GL_SwapWindow_t)dlsym(RTLD_NEXT, "SDL_GL_SwapWindow");

    if (!real_SDL_CreateWindow || !real_SDL_SetWindowTitle || !real_SDL_GL_SwapWindow)
        fprintf(stderr, "[SSTux] Error: Failed to resolve one or more SDL functions\n");
}

/* Hooks */
SDL_Window *SDL_CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags)
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

void SDL_SetWindowTitle(SDL_Window *window, const char *title)
{
    if (!real_SDL_SetWindowTitle)
        return;

    char modifiedTitle[512];
    snprintf(modifiedTitle, sizeof(modifiedTitle), "%s%s", title, " | SSTux ");

    g_Window = window;
    fprintf(stderr, "[SSTux] Hooked SDL_SetWindowTitle, patched window title, updated window: %p\n", g_Window);

    real_SDL_SetWindowTitle(window, modifiedTitle);
}

void SDL_GL_SwapWindow(SDL_Window *window)
{
    if (!real_SDL_GL_SwapWindow)
        return;

    if (window == g_Window && SDL_GL_GetCurrentContext() != NULL)
    {
        DrawGUI(window);
    }

    real_SDL_GL_SwapWindow(window);
}

/* Utils */
SDL_Window *GetStoredWindow(void)
{
    return g_Window;
}