#include <gui/elements/debug_info.hpp>
#include <gui/context.hpp>
#include <config.hpp>

#include <imgui.h>
#include <SDL2/SDL.h>
#include <GL/gl.h>

namespace SSTux::GUI
{
    void RenderDebugInfo()
    {
        ImGui::Begin("Debug Info", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Text("SSTux Version: %s", SSTux::Config::SSTUX_VERSION);

        int w, h;
        SDL_GetWindowSize(GetWindow(), &w, &h);
        ImGui::Text("Window Size: %dx%d", w, h);

        ImVec2 mousePos = ImGui::GetMousePos();
        ImGui::Text("Mouse Position: (%.1f, %.1f)", mousePos.x, mousePos.y);

        SDL_version compiled;
        SDL_VERSION(&compiled);
        ImGui::Text("SDL Version: %d.%d.%d", compiled.major, compiled.minor, compiled.patch);

        const GLubyte *glVersion = glGetString(GL_VERSION);
        ImGui::Text("OpenGL Version: %s", glVersion ? (const char *)glVersion : "Unknown");

        ImGui::Text("Uptime (ms): %u", SDL_GetTicks());

        ImGui::End();
    }
}
