#include <hooks.hpp>
#include <dlfcn.h>
#include <stdexcept>
#include <string>
#include <iostream>
#include <config.hpp>

namespace SSTux::Hooks
{
    namespace
    {
        // SDL functions
        using SDL_CreateWindow_t = SDL_Window *(*)(const char *, int, int, int, int, Uint32);
        using SDL_SetWindowTitle_t = void (*)(SDL_Window *, const char *);
        using SDL_GL_SwapWindow_t = void (*)(SDL_Window *);
        using SDL_GL_CreateContext_t = SDL_GLContext (*)(SDL_Window *);

        // Real SDL function pointers
        SDL_CreateWindow_t real_SDL_CreateWindow = nullptr;
        SDL_SetWindowTitle_t real_SDL_SetWindowTitle = nullptr;
        SDL_GL_SwapWindow_t real_SDL_GL_SwapWindow = nullptr;
        SDL_GL_CreateContext_t real_SDL_GL_CreateContext = nullptr;

        // Global SDL state
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
    }

    void InstallSDLHooks()
    {
        try
        {
            ResolveSymbol(real_SDL_CreateWindow, "SDL_CreateWindow");
            ResolveSymbol(real_SDL_SetWindowTitle, "SDL_SetWindowTitle");
            ResolveSymbol(real_SDL_GL_SwapWindow, "SDL_GL_SwapWindow");
            ResolveSymbol(real_SDL_GL_CreateContext, "SDL_GL_CreateContext");
            Log("SDL hooks installed successfully");
        }
        catch (const std::exception &e)
        {
            Log("Failed to install SDL hooks: " + std::string(e.what()));
            throw;
        }
    }

    // Hook for SDL_CreateWindow
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

        Log("Hooked SDL_CreateWindow, patched title to '" + modifiedTitle + "', window: " + std::to_string(reinterpret_cast<uintptr_t>(window)));
        return window;
    }

    // Hook for SDL_SetWindowTitle
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

        Log("Hooked SDL_SetWindowTitle, patched title to '" + modifiedTitle + "', window: " + std::to_string(reinterpret_cast<uintptr_t>(window)));
    }

    // Hook for SDL_GL_CreateContext
    extern "C" SDL_GLContext SDL_GL_CreateContext(SDL_Window *window)
    {
        if (!real_SDL_GL_CreateContext)
        {
            Log("SDL_GL_CreateContext hook called but real function not resolved");
            return nullptr;
        }

        SDL_GLContext context = real_SDL_GL_CreateContext(window);
        g_GLContext = context;

        Log("Hooked SDL_GL_CreateContext, created context: " + std::to_string(reinterpret_cast<uintptr_t>(context)) + " for window: " + std::to_string(reinterpret_cast<uintptr_t>(window)));
        return context;
    }

    // Utility functions to access stored SDL state
    SDL_Window *GetStoredWindow()
    {
        return g_Window;
    }

    SDL_GLContext GetStoredGLContext()
    {
        return g_GLContext;
    }

} // namespace SSTux::Hooks