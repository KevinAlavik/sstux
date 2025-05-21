#include <gui.hpp>
#include <thread>
#include <chrono>
#include <hooks.hpp>
#include <imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include <SDL2/SDL.h>
#include <stdio.h>

static bool guiReady = false;
static ImGuiIO *guiIO = nullptr;

void InitGUI()
{
    SDL_Window *window = nullptr;

    /* SDL Window init */
    {
        constexpr int maxWaitMs = 5000;
        constexpr int pollIntervalMs = 10;
        int waitedMs = 0;

        while (waitedMs < maxWaitMs)
        {
            window = GetStoredWindow();
            if (window)
                break;

            std::this_thread::sleep_for(std::chrono::milliseconds(pollIntervalMs));
            waitedMs += pollIntervalMs;
        }

        if (!window)
        {
            fprintf(stderr, "[SSTux] Error: Timed out waiting for SDL window.\n");
            return;
        }
    }

    fprintf(stderr, "[SSTux] Got SDL window at %p\n", window);

    /* Check for OpenGL */
    {
        SDL_GLContext context = SDL_GL_GetCurrentContext();
        if (!context)
        {
            fprintf(stderr, "[SSTux] No active OpenGL context. we are cooked bro.\n");
            return;
        }

        fprintf(stderr, "[SSTux] SDL_GLContext is active, OpenGL is used.\n");
    }

    /* ImGui Init */
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO &io = ImGui::GetIO();
        guiIO = &io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        if (!ImGui_ImplSDL2_InitForOpenGL(window, SDL_GL_GetCurrentContext()))
        {
            fprintf(stderr, "[SSTux] ImGui_ImplSDL2_InitForOpenGL failed.\n");
            return;
        }

        if (!ImGui_ImplOpenGL3_Init("#version 130"))
        {
            fprintf(stderr, "[SSTux] ImGui_ImplOpenGL3_Init failed.\n");
            return;
        }

        fprintf(stderr, "[SSTux] ImGui (OpenGL3 backend) initialized successfully.\n");
    }

    fprintf(stderr, "[SSTux] Initialized GUI\n");
    guiReady = true;
}

void DrawGUI(SDL_Window *win)
{
    if (!guiReady || !guiIO)
        return;

    SDL_GL_MakeCurrent(win, SDL_GL_GetCurrentContext());

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Draw UI
    ImGui::Begin("Test Window");
    ImGui::Text("Hello, World!");
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
