#include <gui/context.hpp>
#include <imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include <iostream>
#include <hooks.hpp>

namespace SSTux::GUI
{
    static bool initialized = false;
    static SDL_Window *window = nullptr;
    static SDL_GLContext context = nullptr;

    void Log(const std::string &message)
    {
        std::cerr << "[SSTux] " << message << std::endl;
    }

    bool InitOverlay()
    {
        if (initialized)
            return true;

        window = SSTux::Hooks::GetStoredWindow();
        context = SSTux::Hooks::GetStoredGLContext();

        if (!window || !context)
        {
            Log("Error: SDL window or GL context not set!");
            return false;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplSDL2_InitForOpenGL(window, context);
        ImGui_ImplOpenGL3_Init("#version 130");

        Log("Successfully built overlay");
        initialized = true;
        return true;
    }

    bool IsOverlayInitialized() { return initialized; }
    SDL_Window *GetWindow() { return window; }
    SDL_GLContext GetGLContext() { return context; }
}
