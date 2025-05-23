#include <hooks.hpp>
#include <config.hpp>
#include <gui/gui.hpp>
#include <imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <regex>
#include <sstux.hpp>

namespace SSTux::Hooks
{
    namespace
    {
        // Function pointer types
        using SDL_CreateWindow_t = SDL_Window *(*)(const char *, int, int, int, int, Uint32);
        using SDL_SetWindowTitle_t = void (*)(SDL_Window *, const char *);
        using SDL_GL_SwapWindow_t = void (*)(SDL_Window *);
        using SDL_GL_CreateContext_t = SDL_GLContext (*)(SDL_Window *);
        using SDL_PollEvent_t = int (*)(SDL_Event *);
        using SDL_RenderCopyEx_t = int (*)(SDL_Renderer *, ...);

        // Real function pointers
        SDL_CreateWindow_t real_SDL_CreateWindow = nullptr;
        SDL_SetWindowTitle_t real_SDL_SetWindowTitle = nullptr;
        SDL_GL_SwapWindow_t real_SDL_GL_SwapWindow = nullptr;
        SDL_GL_CreateContext_t real_SDL_GL_CreateContext = nullptr;
        SDL_PollEvent_t real_SDL_PollEvent = nullptr;
        SDL_RenderCopyEx_t real_SDL_RenderCopyEx = nullptr;

        // Stored SDL objects
        SDL_Window *g_Window = nullptr;
        SDL_GLContext g_GLContext = nullptr;

        // SuperTux version info
        int supertux_version_major = -1;
        int supertux_version_minor = -1;
        int supertux_version_patch = -1;
        bool version_parsed = false;

        void ParseVersion(const char *title)
        {
            if (version_parsed || !title)
                return;

            std::string titleStr(title);
            std::smatch match;
            std::regex versionRegex(R"(SuperTux v(\d+)\.(\d+)\.(\d+))");

            if (std::regex_search(titleStr, match, versionRegex) && match.size() == 4)
            {
                supertux_version_major = std::stoi(match[1]);
                supertux_version_minor = std::stoi(match[2]);
                supertux_version_patch = std::stoi(match[3]);
                version_parsed = true;

                Log("Parsed SuperTux version: " +
                    std::to_string(supertux_version_major) + "." +
                    std::to_string(supertux_version_minor) + "." +
                    std::to_string(supertux_version_patch));
            }
            else
            {
                Log("Warning: Could not parse SuperTux version from title.");
            }
        }
    } // anonymous namespace

    void InstallSDLHooks()
    {
        try
        {
            auto &hook_manager = GetHookManager();
            hook_manager.RegisterHook("SDL_CreateWindow", real_SDL_CreateWindow);
            hook_manager.RegisterHook("SDL_SetWindowTitle", real_SDL_SetWindowTitle);
            hook_manager.RegisterHook("SDL_GL_SwapWindow", real_SDL_GL_SwapWindow);
            hook_manager.RegisterHook("SDL_GL_CreateContext", real_SDL_GL_CreateContext);
            hook_manager.RegisterHook("SDL_PollEvent", real_SDL_PollEvent);
            hook_manager.RegisterHook("SDL_RenderCopyEx", real_SDL_RenderCopyEx);
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

        ParseVersion(title);
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

        Update();

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
        (void)renderer;
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

    int GetSuperTuxMajor() { return supertux_version_major; }
    int GetSuperTuxMinor() { return supertux_version_minor; }
    int GetSuperTuxPatch() { return supertux_version_patch; }
    bool HasValidSuperTuxVersion() { return version_parsed; }
} // namespace SSTux::Hooks