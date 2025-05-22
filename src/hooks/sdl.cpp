#include <hooks.hpp>
#include <dlfcn.h>
#include <stdexcept>
#include <string>
#include <iostream>
#include <sstream>
#include <config.hpp>
#include <SDL2/SDL.h>
#include <gui/gui.hpp>
#include <imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include <GL/gl.h>

#define HEXPTR(x) ([](auto ptr) { \
    std::ostringstream oss; \
    oss << "0x" << std::hex << reinterpret_cast<uintptr_t>(ptr); \
    return oss.str(); }(x))

namespace SSTux::Hooks
{
    namespace
    {
        using SDL_CreateWindow_t = SDL_Window *(*)(const char *, int, int, int, int, Uint32);
        using SDL_SetWindowTitle_t = void (*)(SDL_Window *, const char *);
        using SDL_GL_SwapWindow_t = void (*)(SDL_Window *);
        using SDL_GL_CreateContext_t = SDL_GLContext (*)(SDL_Window *);
        using SDL_PollEvent_t = int (*)(SDL_Event *);
        using SDL_RenderCopyEx_t = int (*)(SDL_Renderer *renderer, ...);

        SDL_CreateWindow_t real_SDL_CreateWindow = nullptr;
        SDL_SetWindowTitle_t real_SDL_SetWindowTitle = nullptr;
        SDL_GL_SwapWindow_t real_SDL_GL_SwapWindow = nullptr;
        SDL_GL_CreateContext_t real_SDL_GL_CreateContext = nullptr;
        SDL_PollEvent_t real_SDL_PollEvent = nullptr;
        SDL_RenderCopyEx_t real_SDL_RenderCopyEx = nullptr;

        SDL_Window *g_Window = nullptr;
        SDL_GLContext g_GLContext = nullptr;

        void Log(const std::string &message)
        {
            std::cerr << "[SSTux] " << message << std::endl;
        }

        template <typename T>
        void ResolveSymbol(T &func, const char *symbol)
        {
            void *handle = dlsym(RTLD_NEXT, symbol);
            if (!handle)
            {
                throw std::runtime_error("Failed to resolve symbol: " + std::string(symbol));
            }
            func = reinterpret_cast<T>(handle);
        }

        bool IsUsingOpenGL()
        {
            SDL_GLContext glContext = SDL_GL_GetCurrentContext();
            if (!glContext)
            {
                Log("Error: No active OpenGL context found.");
                return false;
            }

            const char *glVersion = reinterpret_cast<const char *>(glGetString(GL_VERSION));
            if (glVersion)
            {
                Log(std::string("OpenGL version: ") + glVersion);
            }

            return true;
        }

    } // anonymous namespace

    void InstallSDLHooks()
    {
        try
        {
            ResolveSymbol(real_SDL_CreateWindow, "SDL_CreateWindow");
            ResolveSymbol(real_SDL_SetWindowTitle, "SDL_SetWindowTitle");
            ResolveSymbol(real_SDL_GL_SwapWindow, "SDL_GL_SwapWindow");
            ResolveSymbol(real_SDL_GL_CreateContext, "SDL_GL_CreateContext");
            ResolveSymbol(real_SDL_PollEvent, "SDL_PollEvent");
            ResolveSymbol(real_SDL_RenderCopyEx, "SDL_RenderCopyEx");
            Log("SDL hooks installed successfully");
        }
        catch (const std::exception &e)
        {
            Log("Failed to install SDL hooks: " + std::string(e.what()));
            throw;
        }
    }

    extern "C" SDL_Window *SDL_CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags)
    {
        if (!real_SDL_CreateWindow)
        {
            Log("SDL_CreateWindow hook called but real function not resolved");
            return nullptr;
        }

        std::string modifiedTitle = std::string(title) + SSTux::Config::WINDOW_TITLE_PREFIX;
        SDL_Window *window = real_SDL_CreateWindow(modifiedTitle.c_str(), x, y, w, h, flags);
        g_Window = window;

        Log("Hooked SDL_CreateWindow, patched title to '" + modifiedTitle + "', window: " + HEXPTR(window));
        return window;
    }

    extern "C" void SDL_SetWindowTitle(SDL_Window *window, const char *title)
    {
        if (!real_SDL_SetWindowTitle)
        {
            Log("SDL_SetWindowTitle hook called but real function not resolved");
            return;
        }

        std::string modifiedTitle = std::string(title) + SSTux::Config::WINDOW_TITLE_PREFIX;
        real_SDL_SetWindowTitle(window, modifiedTitle.c_str());
        g_Window = window;

        Log("Hooked SDL_SetWindowTitle, patched title to '" + modifiedTitle + "', window: " + HEXPTR(window));
    }

    extern "C" SDL_GLContext SDL_GL_CreateContext(SDL_Window *window)
    {
        if (!real_SDL_GL_CreateContext)
        {
            Log("SDL_GL_CreateContext hook called but real function not resolved");
            return nullptr;
        }

        SDL_GLContext context = real_SDL_GL_CreateContext(window);
        g_GLContext = context;

        Log("Hooked SDL_GL_CreateContext, created context: " + HEXPTR(context) + " for window: " + HEXPTR(window));
        return context;
    }

    extern "C" void SDL_GL_SwapWindow(SDL_Window *window)
    {
        if (!real_SDL_GL_SwapWindow)
        {
            Log("SDL_GL_SwapWindow hook called but real function not resolved");
            return;
        }

        if (window == g_Window)
        {
            SSTux::GUI::DrawOverlay();
        }

        real_SDL_GL_SwapWindow(window);
    }

    extern "C" int SDL_PollEvent(SDL_Event *event)
    {
        if (!real_SDL_PollEvent)
        {
            Log("SDL_PollEvent hook called but real function not resolved");
            return 0;
        }

        int result = real_SDL_PollEvent(event);

        if (result != 0 && event && SSTux::GUI::Ready())
        {
            ImGui_ImplSDL2_ProcessEvent(event);
        }

        return result;
    }

    extern "C" int SDL_RenderCopyEx(SDL_Renderer *renderer, ...)
    {
        /* dont care about real handler */
        Log("Error: SuperTux is not using OpenGL. Exiting.");
        std::exit(EXIT_FAILURE);
        return -1;
    }

    SDL_Window *GetStoredWindow()
    {
        return g_Window;
    }

    SDL_GLContext GetStoredGLContext()
    {
        return g_GLContext;
    }

} // namespace SSTux::Hooks