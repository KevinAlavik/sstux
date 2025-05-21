#include <gui.hpp>
#include <thread>
#include <chrono>
#include <hooks.hpp>
#include <imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <cstdlib>

static bool guiReady = false;
static ImGuiIO *guiIO = nullptr;

void InitGUI()
{
    constexpr int maxWaitMs = 1000;
    constexpr int pollIntervalMs = 10;
    int waitedMs = 0;

    SDL_Window *window = nullptr;

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
        std::exit(EXIT_FAILURE);
    }

    fprintf(stderr, "[SSTux] Got SDL window at %p\n", window);

    waitedMs = 0;

    SDL_GLContext gl_ctx = nullptr;
    while (waitedMs < maxWaitMs)
    {
        if (SDL_GL_GetCurrentContext() != NULL)
            break;

        std::this_thread::sleep_for(std::chrono::milliseconds(pollIntervalMs));
        waitedMs += pollIntervalMs;
    }

    if (SDL_GL_GetCurrentContext() == NULL)
    {
        fprintf(stderr, "[SSTux] Warning: Timed out waiting for OpenGL context, falling back to one fetched from SDL_GL_CreateContext call\n");
        gl_ctx = GetStoredGLContext();
    }
    else
    {
        gl_ctx = SDL_GL_GetCurrentContext();
    }

    if (SDL_GL_MakeCurrent(window, gl_ctx) != 0)
    {
        fprintf(stderr, "[SSTux] SDL_GL_MakeCurrent failed: %s\n", SDL_GetError());
        std::exit(EXIT_FAILURE);
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    guiIO = &io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    if (!ImGui_ImplSDL2_InitForOpenGL(window, gl_ctx))
    {
        fprintf(stderr, "[SSTux] ImGui_ImplSDL2_InitForOpenGL failed.\n");
        std::exit(EXIT_FAILURE);
    }

    if (!ImGui_ImplOpenGL3_Init("#version 310"))
    {
        fprintf(stderr, "[SSTux] ImGui_ImplOpenGL3_Init failed.\n");
        std::exit(EXIT_FAILURE);
    }

    fprintf(stderr, "[SSTux] ImGui (OpenGL3 backend) initialized successfully.\n");
    guiReady = true;
}

void DrawGUI(SDL_Window *win)
{
    if (!guiReady || !guiIO)
        return;

    SDL_GL_MakeCurrent(win, GetStoredGLContext());

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Test Window");
    ImGui::Text("Hello, World!");
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool IsGUIReady()
{
    return guiReady;
}
