#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <SDL2/SDL.h>

/* SDL Hooks */
typedef SDL_Window *(*SDL_CreateWindow_t)(const char *, int, int, int, int, Uint32);
typedef void (*SDL_SetWindowTitle_t)(SDL_Window *, const char *);

/* SDL Real Handlers */
static SDL_CreateWindow_t real_SDL_CreateWindow = NULL;
static SDL_SetWindowTitle_t real_SDL_SetWindowTitle = NULL;

/* Init */
void InstallSDLHooks(void)
{
    real_SDL_CreateWindow = (SDL_CreateWindow_t)dlsym(RTLD_NEXT, "SDL_CreateWindow");
    real_SDL_SetWindowTitle = (SDL_SetWindowTitle_t)dlsym(RTLD_NEXT, "SDL_SetWindowTitle");

    if (!real_SDL_CreateWindow || !real_SDL_SetWindowTitle)
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
    fprintf(stderr, "[SSTux] Hooked SDL_CreateWindow, patched window title\n");
    return win;
}

void SDL_SetWindowTitle(SDL_Window *window, const char *title)
{
    if (!real_SDL_SetWindowTitle)
        return;

    char modifiedTitle[512];
    snprintf(modifiedTitle, sizeof(modifiedTitle), "%s%s", title, " | SSTux ");

    fprintf(stderr, "[SSTux] Hooked SDL_SetWindowTitle, patched window title\n");
    real_SDL_SetWindowTitle(window, modifiedTitle);
}