#include <gui/context.hpp>
#include <imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include <iostream>
#include <hooks.hpp>
#include <SDL2/SDL.h>
#include <GL/gl.h>

namespace SSTux::GUI
{
    namespace
    {
        bool initialized = false;
        SDL_Window *window = nullptr;
        SDL_GLContext context = nullptr;
    }

    // Definition for the extern variable
    OverlaySystemInfo overlayInfo;

    static void Log(const std::string &message)
    {
        std::cerr << "[SSTux] " << message << std::endl;
    }

    bool InitOverlay()
    {
        if (initialized)
            return true;

        window = Hooks::GetStoredWindow();
        context = Hooks::GetStoredGLContext();

        if (!window || !context)
        {
            Log("Error: SDL window or GL context not set!");
            return false;
        }

        if (SDL_GL_MakeCurrent(window, context) != 0)
        {
            Log("Failed to make GL context current: " + std::string(SDL_GetError()));
            return false;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;

        ImGuiStyle &style = ImGui::GetStyle();
        style.WindowRounding = 6.0f;
        style.FrameRounding = 5.0f;
        style.ScrollbarRounding = 5.0f;
        style.GrabRounding = 5.0f;
        style.WindowBorderSize = 1.0f;
        style.FrameBorderSize = 1.0f;
        style.ScrollbarSize = 12.0f;
        style.GrabMinSize = 10.0f;

        ImVec4 *colors = style.Colors;
        colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.105f, 0.11f, 1.0f);
        colors[ImGuiCol_Header] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
        colors[ImGuiCol_Button] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.1f, 0.105f, 0.11f, 1.0f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.105f, 0.11f, 1.0f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
        colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.25f, 0.9f);

        ImGui_ImplSDL2_InitForOpenGL(window, context);
        ImGui_ImplOpenGL3_Init("#version 130");

        SDL_GetWindowSize(window, &overlayInfo.windowWidth, &overlayInfo.windowHeight);
        SDL_VERSION(&overlayInfo.sdlVersion);

        overlayInfo.glVersion = reinterpret_cast<const char *>(glGetString(GL_VERSION));
        overlayInfo.glVendor = reinterpret_cast<const char *>(glGetString(GL_VENDOR));
        overlayInfo.glRenderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
        overlayInfo.glslVersion = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));

        if (!overlayInfo.glVersion || !overlayInfo.glVendor || !overlayInfo.glRenderer)
        {
            Log("Warning: One or more glGetString() calls returned null.");
        }

        Log("Successfully built overlay");
        initialized = true;
        return true;
    }

    bool IsOverlayInitialized() { return initialized; }
    SDL_Window *GetWindow() { return window; }
    SDL_GLContext GetGLContext() { return context; }
}
